if opt_apps.disabled()
  error('Benchmarks require apps')
endif


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
    test_suite = 'ymf262_@0@_play'.format(intr_name)
    foreach score_path : aymo_ymf262_benchmark_suite
      benchmark(
        ('_'.join([test_suite, fs.name(score_path)])).underscorify(),
        aymo_ymf262_play_exe,
        args: [
          '--benchmark',
          '--cpu-ext', intr_name,
          '--loops', '@0@'.format(opt_benchmark_loops),
          score_path
        ],
        timeout: 0
      )
    endforeach
  endif
endforeach

# Strictly run:
#   meson test --benchmark
run_target(
  'benchmark-report',
  command: [
    python_exe,
    join_paths(top_srcdir, 'benchmarks', 'export_report.py'),
    join_paths(top_builddir, 'meson-logs', 'testlog.json'),
    join_paths(top_builddir, 'benchmarks', 'report.csv')
  ]
)
