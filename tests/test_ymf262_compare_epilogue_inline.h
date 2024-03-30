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

#include "aymo.h"


static int app_boot(void)
{
    app_return = TEST_STATUS_HARD;

    aymo_boot();

    score_data = NULL;
    score_size = 0u;
    memset(&score, 0, sizeof(score));

    memset(&nuked_chip, 0, sizeof(nuked_chip));
    memset(&nuked_out, 0, sizeof(nuked_out));
    memset(&aymo_chip, 0, sizeof(aymo_chip));

    return TEST_STATUS_PASS;
}


static int app_args_init(int argc, char** argv)
{
    memset(&app_args, 0, sizeof(app_args));

    app_args.argc = argc;
    app_args.argv = argv;

    app_args.score_type = aymo_score_type_unknown;

    return TEST_STATUS_PASS;
}


static int app_args_parse(void)
{
    if (app_args.argc != 3) {
        fprintf(stderr, "USAGE:\t%s SCORETYPE SCOREPATH\n",
                (app_args.argc ? app_args.argv[0] : "test_exe"));
        return TEST_STATUS_HARD;
    }

    app_args.score_type_cstr = app_args.argv[1];
    app_args.score_type = aymo_score_ext_to_type(app_args.score_type_cstr);
    if (app_args.score_type == aymo_score_type_unknown) {
        fprintf(stderr, "Unsupported score format: %s\n", app_args.score_type_cstr);
        return TEST_STATUS_HARD;
    }

    app_args.score_path_cstr = app_args.argv[2];

    return TEST_STATUS_PASS;
}


static int app_setup(void)
{
    score.base.vt = aymo_score_type_to_vt(app_args.score_type);
    aymo_score_ctor(&score.base);

    if (aymo_file_load(app_args.score_path_cstr, &score_data, &score_size)) {
        perror("aymo_file_load()");
        return TEST_STATUS_HARD;
    }
    if (score_size > UINT32_MAX) {
        fprintf(stderr, "Score size=0x%zX > UINT32_MAX\n", score_size);
        return TEST_STATUS_HARD;
    }
    if (aymo_score_load(&score.base, score_data, (uint32_t)score_size)) {
        fprintf(stderr, "Cannot load score: %s\n", app_args.argv[2]);
        return TEST_STATUS_HARD;
    }

    OPL3_Reset(&nuked_chip, (uint32_t)AYMO_YMF262_SAMPLE_RATE);

    // Fix initial channel gates w.r.t. AYMO
    for (int ch2x = 0; ch2x < 18; ++ch2x) {
        nuked_chip.channel[ch2x].cha = 0xFFFFu;
        nuked_chip.channel[ch2x].chb = 0xFFFFu;
    }

    aymo_(ctor)(&aymo_chip);

    return TEST_STATUS_PASS;
}


static void app_teardown(void)
{
    aymo_(dtor)(&aymo_chip);

    if (score.base.vt) {
        aymo_score_unload(&score.base);
        aymo_score_dtor(&score.base);
    }
    aymo_file_unload(score_data);
    score_data = NULL;
}


static int app_run(void)
{
    struct aymo_score_status* status = aymo_score_get_status(&score.base);

    while (!(status->flags & AYMO_SCORE_FLAG_EOF)) {
        if (compare_chips()) {
            fprintf(stderr, "Chips do not match\n");
            return TEST_STATUS_FAIL;
        }

        aymo_score_tick(&score.base, 1u);

        if (status->flags & AYMO_SCORE_FLAG_EVENT) {
            OPL3_WriteReg(&nuked_chip, status->address, status->value);
            aymo_(write)(&aymo_chip, status->address, status->value);
        }

        OPL3_Generate4Ch(&nuked_chip, &nuked_out[0]);
        aymo_(tick)(&aymo_chip, 1u);
    }
    return TEST_STATUS_PASS;
}


int main(int argc, char** argv)
{
    app_return = app_boot();
    if (app_return) goto catch_;

    app_return = app_args_init(argc, argv);
    if (app_return) goto catch_;

    app_return = app_args_parse();
    if (app_return) goto catch_;

    app_return = app_setup();
    if (app_return) goto catch_;

    app_return = app_run();
    if (app_return) goto catch_;

    app_return = TEST_STATUS_PASS;
    goto finally_;

catch_:
finally_:
    app_teardown();
    return app_return;
}


AYMO_CXX_EXTERN_C_END
