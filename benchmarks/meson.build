if opt_apps.disabled()
  error('Benchmarks require apps')
endif

# =====================================================================
# TDA8425

aymo_tda8425_benchmark_suite = {
  'bass_max':             ['0xFC', '0xFC', '0xFF', '0xF6', '0xFF', '0xCE'],
  'bass_min':             ['0xFC', '0xFC', '0xF0', '0xF6', '0xFF', '0xCE'],
  'dcremoval_lr_max_min': ['0xFF', '0xF0', '0xF6', '0xF6', '0xFF', '0x8E'],
  'dcremoval_lr_min_max': ['0xF0', '0xFF', '0xF6', '0xF6', '0xFF', '0x8E'],
  'mono_1_a':             ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC2'],
  'mono_1_ab':            ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC6'],
  'mono_1_b':             ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC4'],
  'mute':                 ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xEE'],
  'pseudo_1_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFC', '0xD2'],
  'pseudo_1_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFC', '0xD4'],
  'pseudo_2_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFD', '0xD2'],
  'pseudo_2_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFD', '0xD4'],
  'pseudo_3_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFE', '0xD2'],
  'pseudo_3_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFE', '0xD4'],
  'spatial_1_a':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xDE'],
  'spatial_1_ab':         ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xDE'],
  'spatial_1_b':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xDE'],
  'stereo_1_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCA'],
  'stereo_1_ab':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCE'],
  'stereo_1_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCC'],
  'stereo_2_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCB'],
  'stereo_2_ab':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCF'],
  'stereo_2_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCD'],
  'stereo_none_1':        ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC8'],
  'stereo_none_2':        ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC9'],
  'tfilter_max':          ['0xFC', '0xFC', '0xFF', '0xF6', '0xFF', '0x4E'],
  'tfilter_min':          ['0xFC', '0xFC', '0xF0', '0xF6', '0xFF', '0x4E'],
  'treble_max':           ['0xFC', '0xFC', '0xF6', '0xFF', '0xFF', '0xCE'],
  'treble_min':           ['0xFC', '0xFC', '0xF6', '0xF0', '0xFF', '0xCE'],
  'volume_lr_max_min':    ['0xFF', '0xF0', '0xF6', '0xF6', '0xFF', '0xCE'],
  'volume_lr_min_max':    ['0xF0', '0xFF', '0xF6', '0xF6', '0xFF', '0xCE'],
  'worst_case':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFC', '0x12'],
}
test_args_keys = [
  '--reg-vl',
  '--reg-vr',
  '--reg-ba',
  '--reg-tr',
  '--reg-pp',
  '--reg-sf',
]
test_args_idx = [0, 1, 2, 3, 4, 5]

foreach intr_name : ['dummy', 'none', 'x86_sse41', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'tda8425_process_@0@'.format(intr_name)
    foreach test_name, test_args_values : aymo_tda8425_benchmark_suite
      test_args = []
      foreach i : test_args_idx
        test_args += [test_args_keys[i], test_args_values[i]]
      endforeach
      benchmark(
        ('_'.join([test_suite, test_name])).underscorify(),
        aymo_tda8425_process_exe,
        args: [
          '--benchmark',
          '--cpu-ext', intr_name,
          '--buffer-length', '@0@'.format(opt_benchmark_buffer_length),
          '--length', '@0@'.format(opt_benchmark_stream_length),
        ] + test_args,
        timeout: 0
      )
    endforeach
  endif
endforeach

# =====================================================================
# YM7128

aymo_ym7128_benchmark_suite = aymo_ym7128_sweep_suite
test_args_keys = []
foreach i : [1, 2, 3, 4, 5, 6, 7, 8]
  test_args_keys += '--reg-gl@0@'.format(i)
endforeach
foreach i : [1, 2, 3, 4, 5, 6, 7, 8]
  test_args_keys += '--reg-gr@0@'.format(i)
endforeach
test_args_keys += [
  '--reg-vm',
  '--reg-vc',
  '--reg-vl',
  '--reg-vr',
  '--reg-c0',
  '--reg-c1',
]
foreach i : [0, 1, 2, 3, 4, 5, 6, 7, 8]
  test_args_keys += '--reg-t@0@'.format(i)
endforeach
test_args_idx = [
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30
]

foreach intr_name : ['dummy', 'none', 'x86_sse41', 'x86_avx', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'ym7128_process_@0@'.format(intr_name)
    foreach test_name, test_args_values : aymo_ym7128_benchmark_suite
      test_args = []
      foreach i : test_args_idx
        test_args += [test_args_keys[i], test_args_values[i]]
      endforeach
      benchmark(
        ('_'.join([test_suite, test_name])).underscorify(),
        aymo_ym7128_process_exe,
        args: [
          '--benchmark',
          '--cpu-ext', intr_name,
          '--buffer-length', '@0@'.format(opt_benchmark_buffer_length),
          '--length', '@0@'.format(opt_benchmark_stream_length / 2),
        ] + test_args,
        timeout: 0
      )
    endforeach
  endif
endforeach

# =====================================================================
# YMF262

# Some scores stimulating most of the features
# [[format, path], ...]
aymo_ymf262_benchmark_suite = [
  # Real scores
  '../contrib/adplug/test/testmus/BeyondSN.vgm',
  '../contrib/adplug/test/testmus/crusader.raw',
  '../contrib/adplug/test/testmus/doofus.dro',
  '../contrib/adplug/test/testmus/dro_v2.dro',
  '../contrib/adplug/test/testmus/inc.raw',

  # AdPlug reference scores
  '../contrib/adplug/test/testref/bmf1_1.ref',
  '../contrib/adplug/test/testref/bmf1_2.ref',
  '../contrib/adplug/test/testref/dystopia.ref',
  '../contrib/adplug/test/testref/GALWAY.ref',
  '../contrib/adplug/test/testref/loudness.ref',
]

# More scores for deeper testing
if opt_test_all_scores.enabled()
  aymo_ymf262_benchmark_suite += [
    # Real scores
    '../contrib/adplug/test/testmus/MainBGM5.vgm',
    '../contrib/adplug/test/testmus/samurai.dro',
    '../contrib/adplug/test/testmus/WONDERIN.WLF',
    '../contrib/adplug/test/testmus/YsBattle.vgm',

    # AdPlug reference scores
    '../contrib/adplug/test/testref/2.ref',
    '../contrib/adplug/test/testref/2001.ref',
    '../contrib/adplug/test/testref/ACTION.ref',
    '../contrib/adplug/test/testref/ADAGIO.ref',
    '../contrib/adplug/test/testref/adlibsp.ref',
    '../contrib/adplug/test/testref/AKMTEC.ref',
    '../contrib/adplug/test/testref/ALLOYRUN.ref',
    '../contrib/adplug/test/testref/ARAB.ref',
    '../contrib/adplug/test/testref/ARRAKIS.ref',
    '../contrib/adplug/test/testref/BEGIN.ref',
    '../contrib/adplug/test/testref/BeyondSN.ref',
    '../contrib/adplug/test/testref/blaster2.ref',
    '../contrib/adplug/test/testref/BOOTUP.ref',
    '../contrib/adplug/test/testref/canonind.ref',
    '../contrib/adplug/test/testref/CHILD1.ref',
    '../contrib/adplug/test/testref/CHIP.ref',
    '../contrib/adplug/test/testref/crusader.ref',
    '../contrib/adplug/test/testref/DEMO4.ref',
    '../contrib/adplug/test/testref/doofus.ref',
    '../contrib/adplug/test/testref/dro_v2.ref',
    '../contrib/adplug/test/testref/DTM-TRK1.ref',
    '../contrib/adplug/test/testref/DUNE19.ref',
    '../contrib/adplug/test/testref/ending.ref',
    '../contrib/adplug/test/testref/EOBSOUND.ref',
    '../contrib/adplug/test/testref/fdance03.ref',
    '../contrib/adplug/test/testref/flash.ref',
    '../contrib/adplug/test/testref/Flying.ref',
    '../contrib/adplug/test/testref/go-_-go.ref',
    '../contrib/adplug/test/testref/GORBI2.ref',
    '../contrib/adplug/test/testref/GRABBAG.ref',
    '../contrib/adplug/test/testref/HIP_D.ref',
    '../contrib/adplug/test/testref/hybrid.ref',
    '../contrib/adplug/test/testref/hyp.ref',
    '../contrib/adplug/test/testref/ice_thnk.ref',
    '../contrib/adplug/test/testref/inc.ref',
    '../contrib/adplug/test/testref/lines1.ref',
    '../contrib/adplug/test/testref/LOREINTR.ref',
    '../contrib/adplug/test/testref/MainBGM5.ref',
    '../contrib/adplug/test/testref/MARIO.ref',
    '../contrib/adplug/test/testref/menu.ref',
    '../contrib/adplug/test/testref/mi2.ref',
    '../contrib/adplug/test/testref/michaeld.ref',
    '../contrib/adplug/test/testref/MORNING.ref',
    '../contrib/adplug/test/testref/NECRONOM.ref',
    '../contrib/adplug/test/testref/NEWPAGA.ref',
    '../contrib/adplug/test/testref/NEWSAN.ref',
    '../contrib/adplug/test/testref/opensong.ref',
    '../contrib/adplug/test/testref/PLAYMUS1.ref',
    '../contrib/adplug/test/testref/psi1.ref',
    '../contrib/adplug/test/testref/rat.ref',
    '../contrib/adplug/test/testref/REVELAT.ref',
    '../contrib/adplug/test/testref/revival.ref',
    '../contrib/adplug/test/testref/RI051.ref',
    '../contrib/adplug/test/testref/RIK6.ref',
    '../contrib/adplug/test/testref/SAILOR.ref',
    '../contrib/adplug/test/testref/samurai.ref',
    '../contrib/adplug/test/testref/SATNIGHT.ref',
    '../contrib/adplug/test/testref/SCALES.ref',
    '../contrib/adplug/test/testref/SMKEREM.ref',
    '../contrib/adplug/test/testref/SNDTRACK.ref',
    '../contrib/adplug/test/testref/SONG1.ref',
    '../contrib/adplug/test/testref/tafa.ref',
    '../contrib/adplug/test/testref/TEST16.ref',
    '../contrib/adplug/test/testref/TOCCATA.ref',
    '../contrib/adplug/test/testref/TUBES.ref',
    '../contrib/adplug/test/testref/TU_BLESS.ref',
    '../contrib/adplug/test/testref/VIB_VOL3.ref',
    '../contrib/adplug/test/testref/well.ref',
    '../contrib/adplug/test/testref/WONDERIN.ref',
    '../contrib/adplug/test/testref/WORMINTR.ref',
    '../contrib/adplug/test/testref/YsBattle.ref',
  ]
endif

foreach intr_name : ['dummy', 'none', 'x86_sse41', 'x86_avx', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'ymf262_play_@0@'.format(intr_name)
    foreach score_path : aymo_ymf262_benchmark_suite
      benchmark(
        ('_'.join([test_suite, fs.name(score_path)])).underscorify(),
        aymo_ymf262_play_exe,
        args: [
          '--benchmark',
          '--cpu-ext', intr_name,
          '--loops', '@0@'.format(opt_benchmark_score_loops),
          score_path
        ],
        timeout: 0
      )
    endforeach
  endif
endforeach

# =====================================================================
# Strictly run:
#   meson test --benchmark

foreach name : ['tda8425', 'ym7128', 'ymf262']
  run_target(
    'benchmark-report-@0@'.format(name),
    command: [
      python_exe,
      join_paths(top_srcdir, 'benchmarks', 'export_report_@0@.py'.format(name)),
      join_paths(top_builddir, 'meson-logs', 'testlog.json'),
      join_paths(top_builddir, 'benchmarks', 'report_@0@.csv'.format(name))
    ]
  )
endforeach
