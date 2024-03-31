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
        aymo_ymf262_play SCORE | aplay -c 2 -r 49716 -f S16_LE

    - VLC:
        aymo_ymf262_play SCORE | vlc --demux=rawaud --rawaud-channels 2 --rawaud-samplerate 47916 -
*/

#include "aymo.h"
#include "aymo_cpu.h"
#include "aymo_file.h"
#include "aymo_score.h"
#include "aymo_score_dro.h"
#include "aymo_score_imf.h"
#include "aymo_score_raw.h"
#include "aymo_score_ref.h"
#include "aymo_score_vgm.h"
#include "aymo_wave.h"
#include "aymo_ymf262.h"

#include <assert.h>
#include <errno.h>
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


struct app_args {
    int argc;
    char** argv;

    // App parameters
    unsigned loops;
    bool benchmark;

    // Score parameters
    const char* score_path_cstr;        // NULL or "-" for stdin
    const char* score_type_cstr;        // NULL uses score file extension
    enum aymo_score_type score_type;
    unsigned score_after;
    int score_latency;

    // Output parameters
    const char* out_path_cstr;          // NULL or "-" for stdout
    uint32_t out_frame_length;
    bool out_quad;

    // YMF262 parameters
    const struct aymo_ymf262_vt* ymf262_vt;
    bool ymf262_extensions;
};


static int app_return;

static struct app_args app_args;
static clock_t clock_start;
static clock_t clock_end;

static void* score_data;
static size_t score_size;
static union app_scores {
    struct aymo_score_instance base;
    struct aymo_score_dro_instance dro;
    struct aymo_score_imf_instance imf;
    struct aymo_score_raw_instance raw;
    struct aymo_score_ref_instance ref;
    struct aymo_score_vgm_instance vgm;
} score;

static struct aymo_ymf262_chip* chip;

static bool out_stdout;
static FILE* out_file;
static int16_t out_buffer_default[4];
static int16_t* out_buffer_ptr;
static uint32_t out_frame_length;
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


static void aymo_ymf262_write_queued(struct aymo_ymf262_chip* chip_, uint16_t address, uint8_t value)
{
    (void)aymo_ymf262_enqueue_write(chip_, address, value);
}


static int app_boot(void)
{
    app_return = 2;

    aymo_cpu_boot();
    aymo_ymf262_boot();

    score_data = NULL;
    score_size = 0u;
    memset(&score, 0, sizeof(score));

    chip = NULL;

    out_file = NULL;
    out_buffer_ptr = out_buffer_default;
    out_frame_length = 1u;

    return 0;
}


static int app_args_init(int argc, char** argv)
{
    memset(&app_args, 0, sizeof(app_args));

    app_args.argc = argc;
    app_args.argv = argv;

    app_args.loops = 1u;

    app_args.score_type = aymo_score_type_unknown;
    app_args.score_latency = AYMO_YMF262_REG_SAMPLE_LATENCY;

    app_args.out_frame_length = 1u;

    app_args.ymf262_vt = aymo_ymf262_get_best_vt();

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
        if (!strcmp(name, "--help") || !strcmp(name, "-h")) {
            return app_usage();
        }
        if (!strcmp(name, "--benchmark")) {
            app_args.benchmark = true;
            continue;
        }
        if (!strcmp(name, "--out-quad")) {
            app_args.out_quad = true;
            continue;
        }
        if (!strcmp(name, "--ymf62-extensions")) {
            app_args.ymf262_extensions = true;
            continue;
        }

        // Binary options
        if (argi >= (app_args.argc - 1)) {
            break;
        }
        if (!strcmp(name, "--loops")) {
            const char* text = app_args.argv[++argi];
            errno = 0;
            app_args.loops = (unsigned)strtoul(text, NULL, 0);
            if (errno) {
                perror(name);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--score-after")) {
            const char* text = app_args.argv[++argi];
            errno = 0;
            app_args.score_after = (unsigned)strtoul(text, NULL, 0);
            if (errno) {
                perror(name);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--score-type")) {
            const char* value = app_args.argv[++argi];
            app_args.score_type = aymo_score_ext_to_type(value);
            if (app_args.score_type >= aymo_score_type_unknown) {
                fprintf(stderr, "ERROR: Unknown score type \"%s\"\n", value);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--score-latency")) {
            const char* text = app_args.argv[++argi];
            errno = 0;
            app_args.score_latency = (int)strtol(text, NULL, 0);
            if (errno) {
                perror(name);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--cpu-ext")) {
            const char* text = app_args.argv[++argi];
            app_args.ymf262_vt = aymo_ymf262_get_vt(text);
            if (!app_args.ymf262_vt) {
                fprintf(stderr, "ERROR: Unsupported CPU extensions tag: \"%s\"\n", text);
                return 1;
            }
            continue;
        }
        if (!strcmp(name, "--buffer-size")) {
            const char* text = app_args.argv[++argi];
            errno = 0;
            app_args.out_frame_length = (uint32_t)strtoul(text, NULL, 0);
            if (errno) {
                perror(name);
                return 1;
            }
            continue;
        }
        break;
    }

    if (argi == (app_args.argc - 2)) {
        const char* text = app_args.argv[argi++];
        if (!strcmp(text, "-")) {
            text = NULL;
        }
        app_args.score_path_cstr = text;
    }

    if (argi == (app_args.argc - 1)) {
        const char* text = app_args.argv[argi++];
        if (!strcmp(text, "-")) {
            text = NULL;
        }

        if (app_args.score_path_cstr) {
            app_args.out_path_cstr = text;
        }
        else {
            app_args.score_path_cstr = text;
        }
    }


    if (app_args.score_type >= aymo_score_type_unknown) {
        const char* text = app_args.score_path_cstr;
        if (text) {
            const char* ext = strrchr(text, '.');
            if (ext) {
                app_args.score_type = aymo_score_ext_to_type(ext + 1);
            }
        }
        if (app_args.score_type >= aymo_score_type_unknown) {
            fprintf(stderr, "ERROR: Unsupported score type of \"%s\"\n", (text ? text : ""));
            return 1;
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
    if (aymo_file_load(app_args.score_path_cstr, &score_data, &score_size)) {
        return 1;
    }
    score.base.vt = aymo_score_type_to_vt(app_args.score_type);
    if (!score.base.vt) {
        fprintf(stderr, "ERROR: Unsupported score type ID: %d\n", (int)app_args.score_type);
        return 1;
    }
    aymo_score_ctor(&score.base);
    if (aymo_score_load(&score.base, score_data, (uint32_t)score_size)) {
        fprintf(stderr, "ERROR: Cannot load score \"%s\"\n", app_args.score_path_cstr);
        return 1;
    }

    size_t chip_size = app_args.ymf262_vt->get_sizeof();
    void* chip_alignptr = aymo_aligned_alloc(chip_size, 32u);
    if (!chip_alignptr) {
        perror("aymo_aligned_alloc(chip_size)");
        return 2;
    }
    chip = (struct aymo_ymf262_chip*)chip_alignptr;
    chip->vt = app_args.ymf262_vt;
    aymo_ymf262_ctor(chip);

    uint32_t out_channels = (app_args.out_quad ? 4u : 2u);
    out_frame_length = app_args.out_frame_length;
    if (out_frame_length < 1u) {
        out_frame_length = 1u;
    }
    if (out_frame_length > (UINT32_MAX / (sizeof(int16_t) * out_channels))) {
        out_frame_length = (UINT32_MAX / (sizeof(int16_t) * out_channels));
    }
    uint32_t out_buffer_size = (out_frame_length * sizeof(int16_t) * out_channels);
    out_buffer_ptr = (int16_t*)malloc(out_buffer_size);
    if (!out_buffer_ptr) {
        perror("malloc(out_buffer_size)");
        return 2;
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

        aymo_wave_heading_setup(
            &wave_head,
            AYMO_WAVE_FMT_TYPE_PCM,
            (uint16_t)out_channels,
            16u,
            AYMO_YMF262_SAMPLE_RATE,
            0u
        );
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
        aymo_ymf262_dtor(chip);
        aymo_aligned_free(chip);
    }
    chip = NULL;

    if (score.base.vt) {
        aymo_score_unload(&score.base);
        aymo_score_dtor(&score.base);
    }
    aymo_file_unload(score_data);
    score_data = NULL;

    if (!out_stdout && out_file) {
        fclose(out_file);
    }
    out_file = NULL;

    if (out_buffer_ptr && (out_buffer_ptr != out_buffer_default)) {
        free(out_buffer_ptr);
    }
    out_buffer_ptr = NULL;
    out_frame_length = 0u;
}


static int app_run(void)
{
    size_t out_channels = (app_args.out_quad ? 4u : 2u);
    size_t out_sample_length = ((size_t)out_frame_length * out_channels);
    uint32_t frame_total = 0u;
    unsigned pending_loops = (app_args.loops - 1u);
    unsigned score_after = app_args.score_after;

    aymo_ymf262_write_f aymo_ymf262_writer;
    if (app_args.score_latency >= 0) {
        aymo_ymf262_writer = aymo_ymf262_write;
    }
    else {
        aymo_ymf262_writer = aymo_ymf262_write_queued;
    }

    aymo_ymf262_generate_i16x2_f aymo_ymf262_generate_i16;
    if (app_args.out_quad) {
        aymo_ymf262_generate_i16 = aymo_ymf262_generate_i16x4;
    }
    else {
        aymo_ymf262_generate_i16 = aymo_ymf262_generate_i16x2;
    }

    struct aymo_score_status* status = aymo_score_get_status(&score.base);
    bool playing = !(status->flags & AYMO_SCORE_FLAG_EOF);

    clock_start = clock();

    while (playing) {
        int16_t* buffer_ptr = out_buffer_ptr;
        uint32_t avail_length = out_frame_length;
        uint32_t delay_length = status->delay;

        while (avail_length) {
            if (delay_length > avail_length) {
                delay_length = avail_length;
            }

            aymo_ymf262_generate_i16(chip, delay_length, buffer_ptr);
            buffer_ptr += (delay_length * out_channels);
            frame_total++;

            aymo_score_tick(&score.base, delay_length);
            avail_length -= delay_length;

            if (status->flags & AYMO_SCORE_FLAG_EVENT) {
                aymo_ymf262_writer(chip, status->address, status->value);
            }

            while (!(status->flags & (AYMO_SCORE_FLAG_DELAY | AYMO_SCORE_FLAG_EOF))) {
                aymo_score_tick(&score.base, 0u);

                if (status->flags & AYMO_SCORE_FLAG_EVENT) {
                    aymo_ymf262_writer(chip, status->address, status->value);

                    if (app_args.score_latency > 0) {
                        status->delay += (uint32_t)app_args.score_latency;
                        break;
                    }
                }
            }

            if (!(status->flags & AYMO_SCORE_FLAG_EOF)) {
                delay_length = status->delay;
            }
            else if (app_args.loops) {
                if (pending_loops) {
                    pending_loops--;
                    aymo_score_restart(&score.base);
                }
                else if (score_after) {
                    status->flags |= AYMO_SCORE_FLAG_DELAY;
                    status->delay = score_after;
                    delay_length = status->delay;
                    score_after = 0u;
                }
                else {
                    playing = false;
                }
            }
            else {
                aymo_score_restart(&score.base);
            }
        }

        if (out_file) {
            if (fwrite(out_buffer_ptr, sizeof(int16_t), out_sample_length, out_file) != out_sample_length) {
                perror("fwrite(out_buffer)");
                return 2;
            }
        }
    }

    if (out_file && !out_stdout) {
        if (fseek(out_file, 0, SEEK_SET)) {
            perror("fseek(out_file)");
            return 2;
        }
        aymo_wave_heading_setup(
            &wave_head,
            AYMO_WAVE_FMT_TYPE_PCM,
            (uint16_t)out_channels,
            16u,
            AYMO_YMF262_SAMPLE_RATE,
            frame_total
        );
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
