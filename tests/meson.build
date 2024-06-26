test_includes = include_directories(
  '.',
)

test_common_sources = files(
  'aymo_testing.c',
)


test_names = [
]

test_names_none = [
  'test_convert_none',
  'test_tda8425_none_sweep',
  'test_ym7128_none_sweep',
  'test_ymf262_none_compare',
]


test_names_x86 = [
]

test_names_x86_sse41 = [
  'test_convert_x86_sse41',
  'test_tda8425_x86_sse41_sweep',
  'test_ym7128_x86_sse41_sweep',
  'test_ymf262_x86_sse41_compare',
]

test_names_x86_avx = [
  'test_ym7128_x86_avx_sweep',
  'test_ymf262_x86_avx_compare',
]

test_names_x86_avx2 = [
  'test_convert_x86_avx2',
  'test_tda8425_x86_avx2_sweep',
  'test_ymf262_x86_avx2_compare',
]


test_names_arm = [
]

test_names_arm_neon = [
  'test_convert_arm_neon',
  'test_tda8425_arm_neon_sweep',
  'test_ym7128_arm_neon_sweep',
  'test_ymf262_arm_neon_compare',
]


# =====================================================================

fs = import('fs')


# Generic
foreach test_name : test_names
  test_c = '@0@.c'.format(test_name)
  if fs.exists(test_c)
    test_exe = executable(
      test_name,
      test_c,
      test_common_sources,
      c_args: aymo_c_args,
      include_directories: test_includes,
      dependencies: [aymo_static_dep, aymo_libc_dep],
      install: false,
    )
    test(test_name, test_exe)
  endif
endforeach


# x86 specific
if host_cpu_family in ['x86', 'x86_64']
  foreach test_name : test_names_x86
    test_c = '@0@.c'.format(test_name)
    if fs.exists(test_c)
      test_exe = executable(
        test_name,
        test_c,
        test_common_sources,
        c_args: aymo_c_args,
        include_directories: test_includes,
        dependencies: [aymo_static_dep, aymo_libc_dep],
        install: false,
      )
      test(test_name, test_exe)
    endif
  endforeach
endif


# ARM specific
if host_cpu_family in ['arm', 'aarch64']
  foreach test_name : test_names_arm
    test_c = '@0@.c'.format(test_name)
    if fs.exists(test_c)
      test_exe = executable(
        test_name,
        test_c,
        test_common_sources,
        c_args: aymo_c_args,
        include_directories: test_includes,
        dependencies: [aymo_static_dep, aymo_libc_dep],
        install: false,
      )
      test(test_name, test_exe)
    endif
  endforeach
endif


# CPU-ext specific
foreach intr_name : ['none', 'x86_sse41', 'x86_avx', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_names = get_variable('test_names_@0@'.format(intr_name))
    intr_args = get_variable('aymo_@0@_args'.format(intr_name), [])
    foreach test_name : test_names
      test_c = '@0@.c'.format(test_name)
      if fs.exists(test_c)
        test_exe = executable(
          test_name,
          test_c,
          test_common_sources,
          c_args: aymo_c_args + intr_args,
          include_directories: test_includes,
          dependencies: [aymo_static_dep, aymo_libc_dep],
          install: false,
        )
        set_variable('@0@_exe'.format(test_name), test_exe)
      endif
    endforeach
  endif
endforeach


# =====================================================================
# convert

# function_name
aymo_convert_suite = [
  'test_aymo_convert_@0@_i16_f32',
  'test_aymo_convert_@0@_f32_i16',
  'test_aymo_convert_@0@_i16_f32_1',
  'test_aymo_convert_@0@_f32_i16_1',
  'test_aymo_convert_@0@_i16_f32_k',
  'test_aymo_convert_@0@_f32_i16_k',
  'test_aymo_convert_@0@_u16_f32',
  'test_aymo_convert_@0@_f32_u16',
  'test_aymo_convert_@0@_u16_f32_1',
  'test_aymo_convert_@0@_f32_u16_1',
  'test_aymo_convert_@0@_u16_f32_k',
  'test_aymo_convert_@0@_f32_u16_k',
]

foreach intr_name : ['none', 'x86_sse41', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'test_convert_@0@'.format(intr_name)
    test_exe = get_variable('@0@_exe'.format(test_suite))
    foreach t : aymo_convert_suite
      test_name = t.format(intr_name)
      test(test_name, test_exe, args: test_name)
    endforeach
  endif
endforeach


# =====================================================================
# TDA8425

seconds = '8'
samplerate = '48000'
aymo_tda8425_sweep_suite = {
  'bass_max':             ['0xFC', '0xFC', '0xFF', '0xF6', '0xFF', '0xCE', samplerate, seconds],
  'bass_min':             ['0xFC', '0xFC', '0xF0', '0xF6', '0xFF', '0xCE', samplerate, seconds],
  'dcremoval_lr_max_min': ['0xFF', '0xF0', '0xF6', '0xF6', '0xFF', '0x8E', samplerate, seconds],
  'dcremoval_lr_min_max': ['0xF0', '0xFF', '0xF6', '0xF6', '0xFF', '0x8E', samplerate, seconds],
  'mono_1_a':             ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC2', samplerate, seconds],
  'mono_1_ab':            ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC6', samplerate, seconds],
  'mono_1_b':             ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC4', samplerate, seconds],
  'mute':                 ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xEE', samplerate, seconds],
  'pseudo_1_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFC', '0xD2', samplerate, seconds],
  'pseudo_1_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFC', '0xD4', samplerate, seconds],
  'pseudo_2_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFD', '0xD2', samplerate, seconds],
  'pseudo_2_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFD', '0xD4', samplerate, seconds],
  'pseudo_3_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFE', '0xD2', samplerate, seconds],
  'pseudo_3_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFE', '0xD4', samplerate, seconds],
  'spatial_1_a':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xDE', samplerate, seconds],
  'spatial_1_ab':         ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xDE', samplerate, seconds],
  'spatial_1_b':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xDE', samplerate, seconds],
  'stereo_1_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCA', samplerate, seconds],
  'stereo_1_ab_384000':   ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCE',   '384000', seconds],
  'stereo_1_ab_44100':    ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCE',    '44100', seconds],
  'stereo_1_ab_48000':    ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCE',    '48000', seconds],
  'stereo_1_ab_96000':    ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCE',    '96000', seconds],
  'stereo_1_ab':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCE', samplerate, seconds],
  'stereo_1_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCC', samplerate, seconds],
  'stereo_2_a':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCB', samplerate, seconds],
  'stereo_2_ab':          ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCF', samplerate, seconds],
  'stereo_2_b':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xCD', samplerate, seconds],
  'stereo_none_1':        ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC8', samplerate, seconds],
  'stereo_none_2':        ['0xFC', '0xFC', '0xF6', '0xF6', '0xFF', '0xC9', samplerate, seconds],
  'tfilter_max':          ['0xFC', '0xFC', '0xFF', '0xF6', '0xFF', '0x4E', samplerate, seconds],
  'tfilter_min':          ['0xFC', '0xFC', '0xF0', '0xF6', '0xFF', '0x4E', samplerate, seconds],
  'treble_max':           ['0xFC', '0xFC', '0xF6', '0xFF', '0xFF', '0xCE', samplerate, seconds],
  'treble_min':           ['0xFC', '0xFC', '0xF6', '0xF0', '0xFF', '0xCE', samplerate, seconds],
  'volume_lr_max_min':    ['0xFF', '0xF0', '0xF6', '0xF6', '0xFF', '0xCE', samplerate, seconds],
  'volume_lr_min_max':    ['0xF0', '0xFF', '0xF6', '0xF6', '0xFF', '0xCE', samplerate, seconds],
  'worst_case':           ['0xFC', '0xFC', '0xF6', '0xF6', '0xFC', '0x12', samplerate, seconds],
}

foreach intr_name : ['x86_sse41', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'test_tda8425_@0@_sweep'.format(intr_name)
    test_exe = get_variable('@0@_exe'.format(test_suite))
    foreach test_name, test_args : aymo_tda8425_sweep_suite
      test_name = ('_'.join([test_suite, test_name])).underscorify()
      test(test_name, test_exe, args: test_args)
    endforeach
  endif
endforeach


# =====================================================================
# YM7128

seconds = '20'
# name: [seconds, reg...]
aymo_ym7128_sweep_suite = {
  'off': [seconds,
    '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00',
    '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00',
    '0x00', '0x00', '0x00', '0x00',
    '0x00', '0x00',
    '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00'
  ],
  'direct': [seconds,
    '0x3F', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00',
    '0x3F', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00',
    '0x3F', '0x00', '0x3F', '0x3F',
    '0x00', '0x00',
    '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00', '0x00'
  ],
  'dune/arrakis': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x1A', '0x1A',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'dune/baghdad': [seconds,
    '0x1F', '0x00', '0x1B', '0x00', '0x17', '0x00', '0x33', '0x00',
    '0x00', '0x1D', '0x00', '0x19', '0x00', '0x15', '0x00', '0x11',
    '0x1D', '0x1D', '0x1D', '0x1D',
    '0x13', '0x13',
    '0x06', '0x02', '0x04', '0x06', '0x08', '0x0A', '0x0C', '0x0E', '0x10',
  ],
  'dune/morning': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x1B', '0x1B',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'dune/sequence': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x1C', '0x1C',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'dune/sietch': [seconds,
    '0x1F', '0x00', '0x1B', '0x00', '0x17', '0x00', '0x33', '0x00',
    '0x00', '0x1D', '0x00', '0x19', '0x00', '0x15', '0x00', '0x11',
    '0x1D', '0x1D', '0x1D', '0x1D',
    '0x13', '0x13',
    '0x06', '0x02', '0x04', '0x06', '0x08', '0x0A', '0x0C', '0x0E', '0x10',
  ],
  'dune/warsong': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x1C', '0x1C',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'dune/water': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x1A', '0x1A',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'dune/wormintro': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x18', '0x18',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'dune/wormsuit': [seconds,
    '0x18', '0x00', '0x1A', '0x00', '0x1C', '0x00', '0x1E', '0x00',
    '0x00', '0x19', '0x00', '0x1B', '0x00', '0x1D', '0x00', '0x1F',
    '0x1B', '0x1F', '0x17', '0x17',
    '0x12', '0x08',
    '0x1F', '0x07', '0x0A', '0x0D', '0x10', '0x13', '0x16', '0x19', '0x1C',
  ],
  'gold/cavern': [seconds,
    '0x1F', '0x00', '0x1D', '0x00', '0x1B', '0x00', '0x19', '0x00',
    '0x20', '0x3E', '0x20', '0x3C', '0x20', '0x3A', '0x20', '0x38',
    '0x3C', '0x3E', '0x1C', '0x1C',
    '0x11', '0x0A',
    '0x12', '0x10', '0x0E', '0x0C', '0x0A', '0x08', '0x06', '0x04', '0x02',
  ],
  'gold/chapel': [seconds,
    '0x1F', '0x1E', '0x1D', '0x1C', '0x1B', '0x1A', '0x19', '0x18',
    '0x3F', '0x3E', '0x3D', '0x3C', '0x3B', '0x3A', '0x39', '0x38',
    '0x38', '0x3D', '0x1B', '0x1B',
    '0x10', '0x10',
    '0x1F', '0x1F', '0x1D', '0x1B', '0x19', '0x17', '0x15', '0x13', '0x11',
  ],
  'gold/concert_hall': [seconds,
    '0x31', '0x00', '0x15', '0x00', '0x39', '0x00', '0x1D', '0x00',
    '0x00', '0x33', '0x00', '0x17', '0x00', '0x3B', '0x00', '0x1F',
    '0x1A', '0x1C', '0x1D', '0x1D',
    '0x16', '0x16',
    '0x1F', '0x1C', '0x19', '0x16', '0x13', '0x10', '0x0D', '0x0A', '0x07',
  ],
  'gold/deep_space': [seconds,
    '0x18', '0x00', '0x1A', '0x00', '0x1C', '0x00', '0x1E', '0x00',
    '0x00', '0x19', '0x00', '0x1B', '0x00', '0x1D', '0x00', '0x1F',
    '0x1B', '0x1F', '0x1C', '0x1C',
    '0x12', '0x08',
    '0x1F', '0x07', '0x0A', '0x0D', '0x10', '0x13', '0x16', '0x19', '0x1C',
  ],
  'gold/jazz_club': [seconds,
    '0x1F', '0x1B', '0x37', '0x13', '0x2F', '0x0B', '0x27', '0x03',
    '0x1F', '0x3B', '0x17', '0x33', '0x0F', '0x2B', '0x07', '0x23',
    '0x1C', '0x1F', '0x1B', '0x1B',
    '0x0C', '0x0C',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'gold/movie_theater': [seconds,
    '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07', '0x00',
    '0x00', '0x1F', '0x00', '0x17', '0x00', '0x0F', '0x00', '0x07',
    '0x1A', '0x1D', '0x1C', '0x1C',
    '0x16', '0x16',
    '0x1F', '0x03', '0x07', '0x0B', '0x0F', '0x13', '0x17', '0x1B', '0x1F',
  ],
  'gold/recital_hall': [seconds,
    '0x1F', '0x3E', '0x1D', '0x3C', '0x1B', '0x3A', '0x19', '0x38',
    '0x3F', '0x1E', '0x3D', '0x1C', '0x3B', '0x1A', '0x39', '0x18',
    '0x18', '0x1C', '0x1C', '0x1C',
    '0x15', '0x15',
    '0x14', '0x04', '0x06', '0x08', '0x0A', '0x0C', '0x0E', '0x10', '0x12',
  ],
  'gold/stadium': [seconds,
    '0x1F', '0x00', '0x1B', '0x00', '0x17', '0x00', '0x33', '0x00',
    '0x00', '0x1D', '0x00', '0x19', '0x00', '0x15', '0x00', '0x11',
    '0x1D', '0x1D', '0x3D', '0x3D',
    '0x13', '0x13',
    '0x06', '0x02', '0x04', '0x06', '0x08', '0x0A', '0x0C', '0x0E', '0x10',
  ],
}

foreach intr_name : ['x86_sse41', 'x86_avx', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'test_ym7128_@0@_sweep'.format(intr_name)
    test_exe = get_variable('@0@_exe'.format(test_suite))
    foreach test_name, test_args : aymo_ym7128_sweep_suite
      test_name = ('_'.join([test_suite, test_name])).underscorify()
      test(test_name, test_exe, args: test_args)
    endforeach
  endif
endforeach


# =====================================================================
# YMF262

# Some scores stimulating most of the features
# [[format, path], ...]
aymo_ymf262_compare_suite = [
  # Real scores
  ['vgm', '../contrib/adplug/test/testmus/BeyondSN.vgm'],
  ['raw', '../contrib/adplug/test/testmus/crusader.raw'],
  ['dro', '../contrib/adplug/test/testmus/doofus.dro'],
  ['dro', '../contrib/adplug/test/testmus/dro_v2.dro'],
  ['raw', '../contrib/adplug/test/testmus/inc.raw'],

  # AdPlug reference scores
  ['ref', '../contrib/adplug/test/testref/bmf1_1.ref'],
  ['ref', '../contrib/adplug/test/testref/bmf1_2.ref'],
  ['ref', '../contrib/adplug/test/testref/dystopia.ref'],
  ['ref', '../contrib/adplug/test/testref/GALWAY.ref'],
  ['ref', '../contrib/adplug/test/testref/loudness.ref'],
]

# More scores for deeper testing
if opt_test_all_scores.enabled()
  aymo_ymf262_compare_suite += [
    # Real scores
    ['vgm', '../contrib/adplug/test/testmus/MainBGM5.vgm'],
    ['dro', '../contrib/adplug/test/testmus/samurai.dro'],
    ['imf', '../contrib/adplug/test/testmus/WONDERIN.WLF'],
    ['vgm', '../contrib/adplug/test/testmus/YsBattle.vgm'],

    # AdPlug reference scores
    ['ref', '../contrib/adplug/test/testref/2.ref'],
    ['ref', '../contrib/adplug/test/testref/2001.ref'],
    ['ref', '../contrib/adplug/test/testref/ACTION.ref'],
    ['ref', '../contrib/adplug/test/testref/ADAGIO.ref'],
    ['ref', '../contrib/adplug/test/testref/adlibsp.ref'],
    ['ref', '../contrib/adplug/test/testref/AKMTEC.ref'],
    ['ref', '../contrib/adplug/test/testref/ALLOYRUN.ref'],
    ['ref', '../contrib/adplug/test/testref/ARAB.ref'],
    ['ref', '../contrib/adplug/test/testref/ARRAKIS.ref'],
    ['ref', '../contrib/adplug/test/testref/BEGIN.ref'],
    ['ref', '../contrib/adplug/test/testref/BeyondSN.ref'],
    ['ref', '../contrib/adplug/test/testref/blaster2.ref'],
    ['ref', '../contrib/adplug/test/testref/BOOTUP.ref'],
    ['ref', '../contrib/adplug/test/testref/canonind.ref'],
    ['ref', '../contrib/adplug/test/testref/CHILD1.ref'],
    ['ref', '../contrib/adplug/test/testref/CHIP.ref'],
    ['ref', '../contrib/adplug/test/testref/crusader.ref'],
    ['ref', '../contrib/adplug/test/testref/DEMO4.ref'],
    ['ref', '../contrib/adplug/test/testref/doofus.ref'],
    ['ref', '../contrib/adplug/test/testref/dro_v2.ref'],
    ['ref', '../contrib/adplug/test/testref/DTM-TRK1.ref'],
    ['ref', '../contrib/adplug/test/testref/DUNE19.ref'],
    ['ref', '../contrib/adplug/test/testref/ending.ref'],
    ['ref', '../contrib/adplug/test/testref/EOBSOUND.ref'],
    ['ref', '../contrib/adplug/test/testref/fdance03.ref'],
    ['ref', '../contrib/adplug/test/testref/flash.ref'],
    ['ref', '../contrib/adplug/test/testref/Flying.ref'],
    ['ref', '../contrib/adplug/test/testref/go-_-go.ref'],
    ['ref', '../contrib/adplug/test/testref/GORBI2.ref'],
    ['ref', '../contrib/adplug/test/testref/GRABBAG.ref'],
    ['ref', '../contrib/adplug/test/testref/HIP_D.ref'],
    ['ref', '../contrib/adplug/test/testref/hybrid.ref'],
    ['ref', '../contrib/adplug/test/testref/hyp.ref'],
    ['ref', '../contrib/adplug/test/testref/ice_thnk.ref'],
    ['ref', '../contrib/adplug/test/testref/inc.ref'],
    ['ref', '../contrib/adplug/test/testref/lines1.ref'],
    ['ref', '../contrib/adplug/test/testref/LOREINTR.ref'],
    ['ref', '../contrib/adplug/test/testref/MainBGM5.ref'],
    ['ref', '../contrib/adplug/test/testref/MARIO.ref'],
    ['ref', '../contrib/adplug/test/testref/menu.ref'],
    ['ref', '../contrib/adplug/test/testref/mi2.ref'],
    ['ref', '../contrib/adplug/test/testref/michaeld.ref'],
    ['ref', '../contrib/adplug/test/testref/MORNING.ref'],
    ['ref', '../contrib/adplug/test/testref/NECRONOM.ref'],
    ['ref', '../contrib/adplug/test/testref/NEWPAGA.ref'],
    ['ref', '../contrib/adplug/test/testref/NEWSAN.ref'],
    ['ref', '../contrib/adplug/test/testref/opensong.ref'],
    ['ref', '../contrib/adplug/test/testref/PLAYMUS1.ref'],
    ['ref', '../contrib/adplug/test/testref/psi1.ref'],
    ['ref', '../contrib/adplug/test/testref/rat.ref'],
    ['ref', '../contrib/adplug/test/testref/REVELAT.ref'],
    ['ref', '../contrib/adplug/test/testref/revival.ref'],
    ['ref', '../contrib/adplug/test/testref/RI051.ref'],
    ['ref', '../contrib/adplug/test/testref/RIK6.ref'],
    ['ref', '../contrib/adplug/test/testref/SAILOR.ref'],
    ['ref', '../contrib/adplug/test/testref/samurai.ref'],
    ['ref', '../contrib/adplug/test/testref/SATNIGHT.ref'],
    ['ref', '../contrib/adplug/test/testref/SCALES.ref'],
    ['ref', '../contrib/adplug/test/testref/SMKEREM.ref'],
    ['ref', '../contrib/adplug/test/testref/SNDTRACK.ref'],
    ['ref', '../contrib/adplug/test/testref/SONG1.ref'],
    ['ref', '../contrib/adplug/test/testref/tafa.ref'],
    ['ref', '../contrib/adplug/test/testref/TEST16.ref'],
    ['ref', '../contrib/adplug/test/testref/TOCCATA.ref'],
    ['ref', '../contrib/adplug/test/testref/TUBES.ref'],
    ['ref', '../contrib/adplug/test/testref/TU_BLESS.ref'],
    ['ref', '../contrib/adplug/test/testref/VIB_VOL3.ref'],
    ['ref', '../contrib/adplug/test/testref/well.ref'],
    ['ref', '../contrib/adplug/test/testref/WONDERIN.ref'],
    ['ref', '../contrib/adplug/test/testref/WORMINTR.ref'],
    ['ref', '../contrib/adplug/test/testref/YsBattle.ref'],
  ]
endif

foreach intr_name : ['x86_sse41', 'x86_avx', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    test_suite = 'test_ymf262_@0@_compare'.format(intr_name)
    test_exe = get_variable('@0@_exe'.format(test_suite))
    foreach test_args : aymo_ymf262_compare_suite
      test_name = ('_'.join([test_suite, fs.name(test_args[1])])).underscorify()
      test(test_name, test_exe, args: test_args, timeout: 0)
    endforeach
  endif
endforeach
