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

---

To play via shell pipe, run:

    - ALSA Play:
        aymo_ym7128_filter WAVE | aplay -c 2 -r 44100 -f S16_LE

    - VLC:
        aymo_ym7128_filter WAVE | vlc --demux=rawaud --rawaud-channels 2 --rawaud-samplerate 47916 -
*/

#include "aymo.h"
#include "aymo_convert.h"
#include "aymo_cpu.h"
#include "aymo_file.h"
#include "aymo_ym7128.h"
#include "aymo_wave.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if (defined(__WINDOWS__) || defined(__CYGWIN__))
    #include <io.h>
    #include <fcntl.h>
    #ifndef _MSC_VER
        #define _fileno(f)  ((f)->_file)
    #endif
#endif

AYMO_CXX_EXTERN_C_BEGIN


struct app_preset {
    const char* label;
    uint8_t regs[AYMO_YM7128_REG_COUNT];
} const app_presets[] =
{
    { "off", {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }},
    { "direct", {
        0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x3F, 0x00, 0x3F, 0x3F,
        0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }},
    { "dune/arrakis", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x1A, 0x1A,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "dune/baghdad", {
        0x1F, 0x00, 0x1B, 0x00, 0x17, 0x00, 0x33, 0x00,
        0x00, 0x1D, 0x00, 0x19, 0x00, 0x15, 0x00, 0x11,
        0x1D, 0x1D, 0x1D, 0x1D,
        0x13, 0x13,
        0x06, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
    }},
    { "dune/morning", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x1B, 0x1B,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "dune/sequence", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x1C, 0x1C,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "dune/sietch", {
        0x1F, 0x00, 0x1B, 0x00, 0x17, 0x00, 0x33, 0x00,
        0x00, 0x1D, 0x00, 0x19, 0x00, 0x15, 0x00, 0x11,
        0x1D, 0x1D, 0x1D, 0x1D,
        0x13, 0x13,
        0x06, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
    }},
    { "dune/warsong", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x1C, 0x1C,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "dune/water", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x1A, 0x1A,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "dune/wormintro", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x18, 0x18,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "dune/wormsuit", {
        0x18, 0x00, 0x1A, 0x00, 0x1C, 0x00, 0x1E, 0x00,
        0x00, 0x19, 0x00, 0x1B, 0x00, 0x1D, 0x00, 0x1F,
        0x1B, 0x1F, 0x17, 0x17,
        0x12, 0x08,
        0x1F, 0x07, 0x0A, 0x0D, 0x10, 0x13, 0x16, 0x19, 0x1C,
    }},
    { "gold/cavern", {
        0x1F, 0x00, 0x1D, 0x00, 0x1B, 0x00, 0x19, 0x00,
        0x20, 0x3E, 0x20, 0x3C, 0x20, 0x3A, 0x20, 0x38,
        0x3C, 0x3E, 0x1C, 0x1C,
        0x11, 0x0A,
        0x12, 0x10, 0x0E, 0x0C, 0x0A, 0x08, 0x06, 0x04, 0x02,
    }},
    { "gold/chapel", {
        0x1F, 0x1E, 0x1D, 0x1C, 0x1B, 0x1A, 0x19, 0x18,
        0x3F, 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x39, 0x38,
        0x38, 0x3D, 0x1B, 0x1B,
        0x10, 0x10,
        0x1F, 0x1F, 0x1D, 0x1B, 0x19, 0x17, 0x15, 0x13, 0x11,
    }},
    { "gold/concert_hall", {
        0x31, 0x00, 0x15, 0x00, 0x39, 0x00, 0x1D, 0x00,
        0x00, 0x33, 0x00, 0x17, 0x00, 0x3B, 0x00, 0x1F,
        0x1A, 0x1C, 0x1D, 0x1D,
        0x16, 0x16,
        0x1F, 0x1C, 0x19, 0x16, 0x13, 0x10, 0x0D, 0x0A, 0x07,
    }},
    { "gold/deep_space", {
        0x18, 0x00, 0x1A, 0x00, 0x1C, 0x00, 0x1E, 0x00,
        0x00, 0x19, 0x00, 0x1B, 0x00, 0x1D, 0x00, 0x1F,
        0x1B, 0x1F, 0x1C, 0x1C,
        0x12, 0x08,
        0x1F, 0x07, 0x0A, 0x0D, 0x10, 0x13, 0x16, 0x19, 0x1C,
    }},
    { "gold/jazz_club", {
        0x1F, 0x1B, 0x37, 0x13, 0x2F, 0x0B, 0x27, 0x03,
        0x1F, 0x3B, 0x17, 0x33, 0x0F, 0x2B, 0x07, 0x23,
        0x1C, 0x1F, 0x1B, 0x1B,
        0x0C, 0x0C,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "gold/movie_theater", {
        0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07, 0x00,
        0x00, 0x1F, 0x00, 0x17, 0x00, 0x0F, 0x00, 0x07,
        0x1A, 0x1D, 0x1C, 0x1C,
        0x16, 0x16,
        0x1F, 0x03, 0x07, 0x0B, 0x0F, 0x13, 0x17, 0x1B, 0x1F,
    }},
    { "gold/recital_hall", {
        0x1F, 0x3E, 0x1D, 0x3C, 0x1B, 0x3A, 0x19, 0x38,
        0x3F, 0x1E, 0x3D, 0x1C, 0x3B, 0x1A, 0x39, 0x18,
        0x18, 0x1C, 0x1C, 0x1C,
        0x15, 0x15,
        0x14, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12,
    }},
    { "gold/stadium", {
        0x1F, 0x00, 0x1B, 0x00, 0x17, 0x00, 0x33, 0x00,
        0x00, 0x1D, 0x00, 0x19, 0x00, 0x15, 0x00, 0x11,
        0x1D, 0x1D, 0x3D, 0x3D,
        0x13, 0x13,
        0x06, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10,
    }},
    { NULL, {0} }
};


static const char* app_reg_names[AYMO_YM7128_REG_COUNT] =
{
    "gl1",
    "gl2",
    "gl3",
    "gl4",
    "gl5",
    "gl6",
    "gl7",
    "gl8",

    "gr1",
    "gr2",
    "gr3",
    "gr4",
    "gr5",
    "gr6",
    "gr7",
    "gr8",

    "vM",
    "vC",
    "vL",
    "vR",

    "c0",
    "c1",

    "t0",
    "t1",
    "t2",
    "t3",
    "t4",
    "t5",
    "t6",
    "t7",
    "t8"
};


struct app_args {
    int argc;
    char** argv;

    // App parameters
    unsigned buffer_length;
    unsigned length;
    bool benchmark;

    // Input parameters
    const char* in_path_cstr;           // NULL or "-" for stdin
    bool in_float;
    bool in_stereo;

    // Output parameters
    const char* out_path_cstr;          // NULL or "-" for stdout
    bool out_float;

    // YM7128 parameters
    const struct aymo_ym7128_vt* ym7128_vt;
    uint8_t regs[31];
};


static int app_return;

static struct app_args app_args;
static clock_t clock_start;
static clock_t clock_end;

static bool in_stdin;
static bool in_stereo;
static FILE* in_file;
static uint8_t in_buffer_default[sizeof(float) * 2u];
static uint8_t* in_buffer_ptr;

static uint32_t buffer_length;
static struct aymo_ym7128_chip* chip;

static bool out_stdout;
static FILE* out_file;
static uint8_t out_buffer_default[sizeof(float) * 2u];
static uint8_t* out_buffer_ptr;
static struct aymo_wave_heading wave_head;


static void* aymo_aligned_alloc(size_t size, size_t align)
{
    assert(align);
    assert(size < (SIZE_MAX - align - align));

    void* allocptr = calloc((size + align + align), 1u);
    if (allocptr) {
        uintptr_t alignaddr = ((uintptr_t)(void*)allocptr + align);
        uintptr_t offset = (alignaddr % align);
        alignaddr += ((align - offset) % align);
        void* alignptr = (void*)alignaddr;
        uintptr_t refaddr = (alignaddr - sizeof(void*));
        void** refptr = (void**)(void*)refaddr;
        *refptr = allocptr;
        return alignptr;
    }
    return NULL;
}


static void aymo_aligned_free(void* alignptr)
{
    if (alignptr) {
        uintptr_t alignaddr = (uintptr_t)alignptr;
        uintptr_t refaddr = (alignaddr - sizeof(void*));
        void** refptr = (void**)(void*)refaddr;
        void* allocptr = *refptr;
        free(allocptr);
    }
}


static int app_boot(void)
{
    app_return = 2;

    aymo_boot();
    aymo_ym7128_boot();

    buffer_length = 1u;
    chip = NULL;

    in_file = NULL;
    in_buffer_ptr = in_buffer_default;

    out_file = NULL;
    out_buffer_ptr = out_buffer_default;

    return 0;
}


static int app_args_init(int argc, char** argv)
{
    memset(&app_args, 0, sizeof(app_args));

    app_args.argc = argc;
    app_args.argv = argv;

    app_args.buffer_length = 1u;

    app_args.ym7128_vt = aymo_ym7128_get_best_vt();

    for (int i = 0; i < AYMO_YM7128_REG_COUNT; ++i) {
        app_args.regs[i] = app_presets[1].regs[i];
    }

    return 0;
}


static int app_usage(void)
{
    printf("TODO: USAGE\n");

    return -1;  // help
}


static int app_args_parse(void)
{
    int argi;

    for (argi = 1; argi < app_args.argc; ++argi) {
        const char* name = app_args.argv[argi];

        if (!strcmp(name, "--")) {
            ++argi;
            break;
        }

        // Unary options
        if (!strcmp(name, "--benchmark")) {
            app_args.benchmark = true;
            continue;
        }
        if (!strcmp(name, "--help") || !strcmp(name, "-h")) {
            return app_usage();
        }
        if (!strcmp(name, "--in-float")) {
            app_args.in_float = true;
            continue;
        }
        if (!strcmp(name, "--in-stereo")) {
            app_args.in_stereo = true;
            continue;
        }
        if (!strcmp(name, "--out-float")) {
            app_args.out_float = true;
            continue;
        }

        // Binary options
        if (argi >= (app_args.argc - 1)) {
            break;
        }
        if (!strcmp(name, "--buffer-length")) {
            const char* text = app_args.argv[++argi];
            errno = 0;
            app_args.buffer_length = strtoul(text, NULL, 0);
            if (errno) {
                perror(name);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--cpu-ext")) {
            const char* text = app_args.argv[++argi];
            app_args.ym7128_vt = aymo_ym7128_get_vt(text);
            if (!app_args.ym7128_vt) {
                fprintf(stderr, "ERROR: Unsupported CPU extensions tag: \"%s\"\n", text);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--length")) {
            const char* text = app_args.argv[++argi];
            errno = 0;
            app_args.length = strtoul(text, NULL, 0);
            if (errno) {
                perror(name);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--preset")) {
            const char* text = app_args.argv[++argi];
            const struct app_preset* preset = app_presets;
            for (; preset->label; ++preset) {
                if (!strcmp(text, preset->label)) {
                    break;
                }
            }
            if (preset->label) {
                for (int i = 0; i < AYMO_YM7128_REG_COUNT; ++i) {
                    app_args.regs[i] = preset->regs[i];
                }
                continue;
            }
            fprintf(stderr, "ERROR: Unsupported preset name: \"%s\"\n", text);
            return 1;
        }
        if (!strncmp(name, "--reg-", 6)) {
            bool found = false;
            for (int i = 0; i < AYMO_YM7128_REG_COUNT; ++i) {
                if (!strcmp(&name[6], app_reg_names[i])) {
                    const char* text = app_args.argv[++argi];
                    errno = 0;
                    long value = strtol(text, NULL, 16);
                    if (errno || value < 0x00 || value > 0xFF) {
                        fprintf(stderr, "ERROR: Invalid byte value: %s\n", text);
                        return 1;
                    }
                    app_args.regs[i] = (uint8_t)value;
                    found = true;
                    break;
                }
            }
            if (found) {
                continue;
            }
        }
        break;
    }

    if (argi == (app_args.argc - 2)) {
        const char* text = app_args.argv[argi++];
        if (!strcmp(text, "-")) {
            text = NULL;
        }
        app_args.in_path_cstr = text;
    }

    if (argi == (app_args.argc - 1)) {
        const char* text = app_args.argv[argi++];
        if (!strcmp(text, "-")) {
            text = NULL;
        }

        if (app_args.in_path_cstr) {
            app_args.out_path_cstr = text;
        }
        else {
            app_args.in_path_cstr = text;
        }
    }

    if (argi < app_args.argc) {
        fprintf(stderr, "ERROR: Unknown options after #%d = \"%s\"\n", argi, app_args.argv[argi]);
        return 1;
    }

    return 0;
}


static int app_setup(void)
{
    size_t chip_size = app_args.ym7128_vt->get_sizeof();
    void* chip_alignptr = aymo_aligned_alloc(chip_size, 32u);
    if (!chip_alignptr) {
        perror("aymo_aligned_alloc(chip_size)");
        return 2;
    }
    chip = (struct aymo_ym7128_chip*)chip_alignptr;
    chip->vt = app_args.ym7128_vt;
    aymo_ym7128_ctor(chip);

    for (int i = 0; i < AYMO_YM7128_REG_COUNT; ++i) {
        aymo_ym7128_write(chip, (uint16_t)i, app_args.regs[i]);
    }

    buffer_length = app_args.buffer_length;
    if (buffer_length != 1u) {
        if (buffer_length < 1u) {
            buffer_length = 1u;
        }
        if (buffer_length > (UINT32_MAX / (sizeof(float) * 2u))) {
            buffer_length = (UINT32_MAX / (sizeof(float) * 2u));
        }
        size_t buffer_size = (buffer_length * (sizeof(float) * 2u));
        in_buffer_ptr = (uint8_t*)malloc(buffer_size);
        if (!in_buffer_ptr) {
            perror("malloc(in_buffer_size)");
            return 2;
        }
        out_buffer_ptr = (uint8_t*)malloc(buffer_size);
        if (!out_buffer_ptr) {
            perror("malloc(out_buffer_size)");
            return 2;
        }
    }

    if (app_args.benchmark) {
        in_stdin = false;
        in_file = NULL;
    }
    else if (!app_args.in_path_cstr || !strcmp(app_args.in_path_cstr, "") || !strcmp(app_args.in_path_cstr, "-")) {
        in_stdin = true;
        in_file = stdout;

        #if (defined(__WINDOWS__) || defined(__CYGWIN__))
            errno = 0;
            _setmode(_fileno(stdin), O_BINARY);
            if (errno) {
                perror("_setmode(stdin)");
                return 2;
            }
        #endif
    }
    else {
        in_stdin = false;
        in_file = fopen(app_args.in_path_cstr, "rb");
        if (!in_file) {
            perror(app_args.in_path_cstr);
            return 1;
        }
    }

    if (app_args.benchmark) {
        out_stdout = false;
        out_file = NULL;
    }
    else if (!app_args.out_path_cstr || !strcmp(app_args.out_path_cstr, "") || !strcmp(app_args.out_path_cstr, "-")) {
        out_stdout = true;
        out_file = stdout;

        #if (defined(__WINDOWS__) || defined(__CYGWIN__))
            errno = 0;
            _setmode(_fileno(stdout), O_BINARY);
            if (errno) {
                perror("_setmode(stdout)");
                return 2;
            }
        #endif
    }
    else {
        out_stdout = false;
        out_file = fopen(app_args.out_path_cstr, "wb");
        if (!out_file) {
            perror(app_args.out_path_cstr);
            return 1;
        }

        if (app_args.out_float) {
            aymo_wave_heading_setup(&wave_head, AYMO_WAVE_FMT_TYPE_FLOAT,
                                    2u, 32u, (uint32_t)AYMO_YM7128_OUTPUT_RATE, 0u);
        }
        else {
            aymo_wave_heading_setup(&wave_head, AYMO_WAVE_FMT_TYPE_PCM,
                                    2u, 16u, (uint32_t)AYMO_YM7128_OUTPUT_RATE, 0u);
        }
        if (fwrite(&wave_head, sizeof(wave_head), 1u, out_file) != 1u) {
            perror("fwrite(wave_head)");
            return 2;
        }
    }

    return 0;
}


static void app_teardown(void)
{
    if (chip) {
        aymo_ym7128_dtor(chip);
        aymo_aligned_free(chip);
    }
    chip = NULL;

    if (!in_stdin && in_file) {
        fclose(in_file);
    }
    in_file = NULL;

    if (in_buffer_ptr && (in_buffer_ptr != in_buffer_default)) {
        free(in_buffer_ptr);
    }
    in_buffer_ptr = NULL;

    if (!out_stdout && out_file) {
        fclose(out_file);
    }
    out_file = NULL;

    if (out_buffer_ptr && (out_buffer_ptr != out_buffer_default)) {
        free(out_buffer_ptr);
    }
    out_buffer_ptr = NULL;

    buffer_length = 0u;
}


static int app_run(void)
{
    size_t pending_length = app_args.length;
    size_t sample_length = buffer_length;
    size_t frame_total = 0u;
    size_t in_sample_size = (app_args.in_float ? sizeof(float) : sizeof(int16_t));
    size_t out_sample_size = (app_args.out_float ? sizeof(float) : sizeof(int16_t));
    uint8_t* in_ptr = in_buffer_ptr;
    uint8_t* out_ptr = out_buffer_ptr;

    clock_start = clock();

    for (;;) {
        size_t avail_length = sample_length;
        if (pending_length) {
            if (avail_length > pending_length) {
                avail_length = pending_length;
            }
        }

        if (in_file) {
            avail_length = fread(in_ptr, in_sample_size, avail_length, in_file);
            if (avail_length == 0u) {
                break;
            }
            if (app_args.in_float) {
                aymo_convert_f32_i16_1(avail_length, (float*)in_ptr, (int16_t*)out_ptr);
                uint8_t* t = in_ptr;
                in_ptr = out_ptr;
                out_ptr = t;
            }
            if (in_stereo) {
                for (unsigned i = 0u; i < avail_length; i += 2u) {
                    ((int16_t*)in_ptr)[i / 2u] = ((int16_t*)in_ptr)[i];  // left only
                }
            }
        }

        aymo_ym7128_process_i16(chip, (uint32_t)avail_length, (int16_t*)in_ptr, (int16_t*)out_ptr);

        if (out_file) {
            if (app_args.out_float) {
                aymo_convert_i16_f32_1(avail_length, (int16_t*)out_ptr, (float*)in_ptr);
                uint8_t* t = in_ptr;
                in_ptr = out_ptr;
                out_ptr = t;
            }
            if (fwrite(out_ptr, out_sample_size, (avail_length * 2u), out_file) != avail_length) {
                perror("fwrite(out_buffer)");
                return 2;
            }
        }

        frame_total += avail_length;
        if (pending_length) {
            pending_length -= avail_length;
            if (!pending_length) {
                break;
            }
        }
    }

    if (out_file && !out_stdout) {
        if (fseek(out_file, 0, SEEK_SET)) {
            perror("fseek(out_file)");
            return 2;
        }
        if (app_args.out_float) {
            aymo_wave_heading_setup(&wave_head, AYMO_WAVE_FMT_TYPE_FLOAT, 2u, 32u,
                                    (uint32_t)AYMO_YM7128_OUTPUT_RATE, (uint32_t)frame_total);
        }
        else {
            aymo_wave_heading_setup(&wave_head, AYMO_WAVE_FMT_TYPE_PCM, 2u, 16u,
                                    (uint32_t)AYMO_YM7128_OUTPUT_RATE, (uint32_t)frame_total);
        }
        if (fwrite(&wave_head, sizeof(wave_head), 1u, out_file) != 1u) {
            perror("fwrite(wave_head)");
            return 2;
        }
    }

    clock_end = clock();

    if (app_args.benchmark) {
        clock_t clock_duration = (clock_end - clock_start);
        double seconds = ((double)clock_duration * (1. / (double)CLOCKS_PER_SEC));
        printf("Render time: %.6f seconds\n", seconds);
    }

    return 0;
}


int main(int argc, char** argv)
{
    app_return = app_boot();
    if (app_return) goto catch_;

    app_return = app_args_init(argc, argv);
    if (app_return) goto catch_;

    app_return = app_args_parse();
    if (app_return == -1) {  // help
        app_return = 0;
        goto finally_;
    }
    if (app_return) goto catch_;

    app_return = app_setup();
    if (app_return) goto catch_;

    app_return = app_run();
    if (app_return) goto catch_;

    goto finally_;

catch_:
finally_:
    app_teardown();
    return app_return;
}


AYMO_CXX_EXTERN_C_END
