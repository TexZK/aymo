# This Meson build script is a heavily modified version of the
# "/meson.build" file of the OPUS codec project, adapted to
# suit AYMO.
# That script is used as a template for AYMO because it has a
# known and tested support for SIMD auto-detection within Meson.
# This way, any changes by the OPUS project can be applied to AYMO.
#
# OPUS project reference links:
#   https://opus-codec.org/
#   https://github.com/xiph/opus/
#
# Reference file snapshot:
#   https://github.com/xiph/opus/blob/20c032d27c59d65b19b8ffbb2608e5282fe817eb/meson.build
#
# OPUS license disclaimer:
# --- BEGIN OPUS LICENSE ---
#
# Copyright 2001-2011 Xiph.Org, Skype Limited, Octasic,
#                     Jean-Marc Valin, Timothy B. Terriberry,
#                     CSIRO, Gregory Maxwell, Mark Borgerding,
#                     Erik de Castro Lopo
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution.
#
# - Neither the name of Internet Society, IETF or IETF Trust, nor the
# names of specific contributors, may be used to endorse or promote
# products derived from this software without specific prior written
# permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# --- END OPUS LICENSE ---

# =====================================================================

project('aymo', 'c',
  version: '0.0.3',
  meson_version: '>=0.60.0',
  default_options: [
    'warning_level=2',
    'c_std=c99',
    'buildtype=release'
  ],
)
libversion = '0.0.3'

cc = meson.get_compiler('c')
host_system = host_machine.system()
host_cpu_family = host_machine.cpu_family()
top_srcdir = meson.current_source_dir()
top_builddir = meson.current_build_dir()

aymo_includes = include_directories('.', 'include')

python_exe = find_program('python', 'python3')

# =====================================================================

add_project_arguments('-DAYMO_BUILD=1', language: 'c')
add_project_arguments('-DAYMO_HAVE_CONFIG_H=1', language: 'c')

if host_system == 'windows'
  if cc.get_argument_syntax() == 'msvc'
    add_project_arguments('-D_CRT_SECURE_NO_WARNINGS=1', language: 'c')
  endif
endif

if cc.get_argument_syntax() == 'gcc'
  #add_project_arguments('-D_FORTIFY_SOURCE=2', language: 'c')
endif

# Check for extra compiler args
additional_c_args = []
if cc.get_argument_syntax() == 'msvc'
  additional_c_args += [
  ]
else  # msvc
  additional_c_args += [
    '-fvisibility=hidden',
    '-Wcast-align',
    '-Wnested-externs',
    '-Wshadow',
    '-Wstrict-prototypes',
  ]

  # On Windows, -fstack-protector-strong adds a libssp-0.dll dependency and
  # prevents static linking
  if host_system != 'windows'
    #additional_c_args += ['-fstack-protector-strong']
  endif
endif  # msvc

foreach arg : additional_c_args
  if cc.has_argument(arg)
    add_project_arguments(arg, language: 'c')
  endif
endforeach

# Windows MSVC warnings
if cc.get_id() == 'msvc'
  # Ignore several spurious warnings.
  # If a warning is completely useless and spammy, use '/wdXXXX' to suppress it
  # If a warning is harmless but hard to fix, use '/woXXXX' so it's shown once
  # NOTE: Only add warnings here if you are sure they're spurious
#  add_project_arguments('/wd4035', '/wd4715', '/wd4116', '/wd4046', '/wd4068',
#    '/wd4820', '/wd4244', '/wd4255', '/wd4668',
#    language : 'c')
endif

# =====================================================================

aymo_version = meson.project_version()
aymo_url = 'https://github.com/TexZK/aymo/'

aymo_conf = configuration_data()
aymo_conf.set('PACKAGE_BUGREPORT', '"texzk@email.it"')
aymo_conf.set('PACKAGE_NAME', '"aymo"')
aymo_conf.set('PACKAGE_STRING', '"aymo @0@"'.format(aymo_version))
aymo_conf.set('PACKAGE_TARNAME', '"aymo"')
aymo_conf.set('PACKAGE_URL', '"@0@"'.format(aymo_url))
aymo_conf.set('PACKAGE_VERSION', '"@0@"'.format(aymo_version))

aymo_conf.set('AYMO_CC_HOST_@0@'.format(host_system.underscorify().to_upper()), 1)
aymo_conf.set('AYMO_CC_ID_@0@'.format(cc.get_id().underscorify().to_upper()), 1)
aymo_conf.set('AYMO_CC_SYNTAX_@0@'.format(cc.get_argument_syntax().underscorify().to_upper()), 1)

if cc.check_header('stdint.h')
  aymo_conf.set('AYMO_CC_HAVE_STDINT_H', 1)
endif

opt_apps = get_option('apps')
opt_asm = get_option('asm')
opt_benchmarks = get_option('benchmarks')
opt_benchmark_score_loops = get_option('benchmark_score_loops')
opt_benchmark_buffer_length = get_option('benchmark_buffer_length')
opt_benchmark_stream_length = get_option('benchmark_stream_length')
opt_docs = get_option('docs')
opt_rtcd = get_option('rtcd')
opt_tests = get_option('tests')
opt_test_all_scores = get_option('test_all_scores')

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG=1', language : 'c')
  add_project_arguments('-D_DEBUG=1', language : 'c')
  add_project_arguments('-DAYMO_DEBUG=1', language : 'c')
else
  add_project_arguments('-DNDEBUG=1', language : 'c')
endif

# =====================================================================

aymo_conf.set('AYMO_CPU_FAMILY_@0@'.format(host_cpu_family.underscorify().to_upper()), 1)

# With GCC, Clang, ICC, etc, we differentiate between
# 'runtime support for this SIMD' and 'presume we have this SIMD',
# by checking whether the SIMD / intrinsics can be compiled by the compiler
# as-is ('presume') or with SIMD cflags ('support').
#
# With MSVC, the compiler will always build SIMD/intrinsics targeting all
# specific instruction sets supported by that version of the compiler.
# No special arguments are ever needed.
#
# If runtime CPU detection is not disabled, we must always assume that
# we only have runtime 'support' for it.

aymo_can_presume_simd = true
if cc.get_argument_syntax() == 'msvc'
  if opt_rtcd.disabled()
    warning('Building with an MSVC-like compiler and runtime CPU detection is disabled. Outputs may not run on all @0@ CPUs.'.format(host_cpu_family))
  else
    aymo_can_presume_simd = false
  endif
endif

# TODO: NEON has 'hardfp' vs 'softfp' compiler configuration issues.
# When targeting 'AArch32 softfp', we sometimes need to explicitly pass
# '-mfloat-abi=softfp' to enable NEON (e.g. on Android).
# It should be set in the cross file.
arm_neon_link_args = []
if cc.get_argument_syntax() != 'msvc'
  arm_neon_link_args += ['-mfpu=neon']
endif

aymo_have_dummy = true  # always
aymo_have_none = true  # always

aymo_have_x86_sse = false
aymo_have_x86_sse2 = false
aymo_have_x86_sse41 = false
aymo_have_x86_avx = false
aymo_have_x86_avx2 = false

aymo_have_arm_neon = false

rtcd_support_names = []
intrin_support_names = []

if host_cpu_family in ['arm', 'aarch64']
  # Check for ARMv7/AArch64 neon intrinsics
  intrin_check_code = '''
    #include <arm_neon.h>
    int main(void) {
      static float32x4_t A0, A1, SUMM;
      SUMM = vmlaq_f32(SUMM, A0, A1);
      return (int)vgetq_lane_f32(SUMM, 0);
    }
  '''
  intrin_name = 'ARMv7/AArch64 NEON'
  if cc.links(intrin_check_code,
              name: 'compiler supports @0@ intrinsics'.format(intrin_name))
    aymo_arm_presume_neon = aymo_can_presume_simd
    aymo_arm_support_neon = true
  else
    aymo_arm_presume_neon = false
    if cc.links(intrin_check_code,
                args: arm_neon_link_args,
                name: 'compiler supports @0@ intrinsics with @1@'
                      .format(intrin_name, ' '.join(arm_neon_link_args)))
      aymo_arm_support_neon = true
    else
      aymo_arm_support_neon = false
    endif
  endif

  if aymo_arm_support_neon
    aymo_have_arm_neon = true
    intrin_support_names += [intrin_name]
    aymo_conf.set('AYMO_CPU_SUPPORT_ARM_NEON', 1)
    if aymo_arm_presume_neon
      aymo_conf.set('AYMO_CPU_PRESUME_ARM_NEON', 1)
    else
      rtcd_support_names += [intrin_name]
      aymo_arm_neon_args = arm_neon_link_args
    endif
  else
    message('Compiler does not support @0@ intrinsics'.format(intrin_name))
  endif

  # Check for aarch64 neon intrinsics
  intrin_check_code = '''
    #include <arm_neon.h>
    int main(void) {
      static int32_t x;
      static int16_t y;
      y = vqmovns_s32(x);
    }
  '''
  intrin_name = 'AArch64 NEON'
  if cc.links(intrin_check_code,
              name: 'compiler supports @0@ intrinsics'.format(intrin_name))
    aymo_arm_presume_aarch64 = aymo_can_presume_simd
    aymo_arm_support_aarch64 = true
  else
    aymo_arm_presume_aarch64 = false
    if cc.links(intrin_check_code,
                args: arm_neon_link_args,
                name: 'compiler supports @0@ intrinsics with @1@'
                      .format(intrin_name, ' '.join(arm_neon_link_args)))
      aymo_arm_support_aarch64 = true
    else
      aymo_arm_support_aarch64 = false
    endif
  endif

  if aymo_arm_support_aarch64
    intrin_support_names += [intrin_name]
    aymo_conf.set('AYMO_CPU_SUPPORT_ARM_AARCH64', 1)
    if aymo_arm_presume_aarch64
      aymo_conf.set('AYMO_CPU_PRESUME_ARM_AARCH64', 1)
    endif
  else
    message('Compiler does not support @0@ intrinsics'.format(intrin_name))
  endif

elif host_cpu_family in ['x86', 'x86_64']
  # allow external override/specification of the flags
  x86_intrinsics = [
    [ 'x86_sse',   'SSE',    'xmmintrin.h', '__m128',  '_mm_setzero_ps()',                                     [['-msse'],    ['/arch:SSE']]  ],
    [ 'x86_sse2',  'SSE2',   'emmintrin.h', '__m128i', '_mm_setzero_si128()',                                  [['-msse2'],   ['/arch:SSE2']] ],
    [ 'x86_sse41', 'SSE4.1', 'smmintrin.h', '__m128i', '_mm_setzero_si128(); x = _mm_cmpeq_epi64(x, x)',       [['-msse4.1'], ['/arch:SSE2']] ],
    [ 'x86_avx',   'AVX',    'immintrin.h', '__m256',  '_mm256_setzero_ps()',                                  [['-mavx'],    ['/arch:AVX']]  ],
    [ 'x86_avx2',  'AVX2',   'immintrin.h', '__m256i', '_mm256_setzero_si256(); x = _mm256_cmpeq_epi16(x, x)', [['-mavx2'],   ['/arch:AVX2']] ],
  ]
  foreach intrin : x86_intrinsics
    intrin_check_code = '''
      #include <@0@>
      int main(void) {
        @1@ x;
        x = @2@;
        return 0;
      }
    '''.format(intrin[2], intrin[3], intrin[4])
    intrin_name = intrin[1]
    # Intrinsics arguments are not available with MSVC-like compilers
    intrin_args = ((cc.get_argument_syntax() == 'msvc') ? intrin[5][1] : intrin[5][0])
    if cc.links(intrin_check_code,
                name: 'compiler supports @0@ intrinsics'.format(intrin_name))
      support_intrin = true
      presume_intrin = aymo_can_presume_simd
    elif intrin_args.length() > 0
      presume_intrin = false
      support_intrin = false
      if cc.links(intrin_check_code,
                  args: intrin_args,
                  name: 'compiler supports @0@ intrinsics with @1@'
                        .format(intrin_name, ' '.join(intrin_args)))
        support_intrin = true
      endif
    endif  # intrin_check_code
    if support_intrin
      intrin_support_names += [intrin_name]
      intrin_lower_name = intrin[0]
      set_variable('aymo_have_@0@'.format(intrin_lower_name), true)
      intrin_upper_name = intrin_lower_name.to_upper()
      aymo_conf.set('AYMO_CPU_SUPPORT_@0@'.format(intrin_upper_name), 1)
      if presume_intrin
        aymo_conf.set('AYMO_CPU_PRESUME_@0@'.format(intrin_upper_name), 1)
      else
        rtcd_support_names += [intrin_name]
        set_variable('aymo_@0@_args'.format(intrin_lower_name), intrin_args)
      endif
    else
      message('Compiler does not support @0@ intrinsics'.format(intrin_name))
    endif  # support_intrin
  endforeach  # intrin

  if not opt_rtcd.disabled()
    cpuid_h__cpuid_code = '''
      #include <cpuid.h>
      int main(void) {
        unsigned e1[4] = { 0u, 0u, 0u, 0u };
        __cpuid(1u, e1[0], e1[1], e1[2], e1[3]);
        return 0;
      }
    '''
    cpuid_h__cpuid_count_code = '''
      #include <cpuid.h>
      int main(void) {
        unsigned e7[4] = { 0u, 0u, 0u, 0u };
        __cpuid_count(7u, 0u, e7[0], e7[1], e7[2], e7[3]);
        return 0;
      }
    '''
    intrin_h__cpuid_code = '''
      #include <intrin.h>
      int main(void) {
        int e1[4] = { 0, 0, 0, 0 };
        __cpuid(e1, 1);
        return 0;
      }
    '''
    intrin_h__cpuidex_code = '''
      #include <intrin.h>
      int main(void) {
        int e7[4] = { 0, 0, 0, 0 };
        __cpuidex(e7, 7, 0);
        return 0;
      }
    '''
    have_cpuinfo = false
    if cc.links(cpuid_h__cpuid_code, name: '<cpuid.h> __cpuid()')
      aymo_conf.set('AYMO_CPU_HAVE_CPUINFO_CPUID_H_CPUID', 1)
      have_cpuinfo = true
    endif
    if cc.links(cpuid_h__cpuid_count_code, name: '<cpuid.h> __cpuid_count()')
      aymo_conf.set('AYMO_CPU_HAVE_CPUINFO_CPUID_H_CPUID_COUNT', 1)
      have_cpuinfo = true
    endif
    if cc.links(intrin_h__cpuid_code, name: '<intrin.h> __cpuid()')
      aymo_conf.set('AYMO_CPU_HAVE_CPUINFO_INTRIN_H_CPUID', 1)
      have_cpuinfo = true
    endif
    if cc.links(intrin_h__cpuidex_code, name: '<intrin.h> __cpuidex()')
      aymo_conf.set('AYMO_CPU_HAVE_CPUINFO_INTRIN_H_CPUIDEX', 1)
      have_cpuinfo = true
    endif
    if have_cpuinfo
      aymo_conf.set('AYMO_CPU_HAVE_CPUINFO', 1)
    else
      if opt_rtcd.enabled()
        error('rtcd option is enabled, but no Get CPU Info method detected')
      endif
      warning('Get CPU Info method not detected, no rtcd for intrinsics')
    endif
  endif  # opt_rtcd

  aymo_conf.set('AYMO_CPU_X86_AVX2_GATHER16_STRATEGY', 2)  # TODO: option()

else  # host_cpu_family
  warning('No intrinsics support for @0@'.format(host_cpu_family))
endif  # host_cpu_family

# Check whether we require intrinsics and we support intrinsics on this cpu,
# but none were detected. Can happen because of incorrect compiler flags, such
# as missing -mfloat-abi=softfp on ARM32 softfp cpuitectures.
if intrin_support_names.length() == 0
  warning('"intrinsics" option was enabled, but none were detected')
endif

if opt_rtcd.disabled()
  rtcd_support_names = 'disabled'
else
  if rtcd_support_names.length() > 0
    aymo_conf.set('AYMO_CPU_HAVE_RTCD', 1)
  else
    if intrin_support_names.length() == 0
      rtcd_support_names = 'none'
      if opt_rtcd.enabled()
        warning('"rtcd" option is enabled, but no support for intrinsics is available')
      endif
    else
      rtcd_support_names = 'not needed'
    endif
  endif
endif  # opt_rtcd

# =====================================================================

sources = {
  'AYMO_SOURCES': files(
    'src/aymo.c',
    'src/aymo_convert.c',
    'src/aymo_convert_none.c',
    'src/aymo_cpu.c',
    'src/aymo_score.c',
    'src/aymo_score_dro.c',
    'src/aymo_score_imf.c',
    'src/aymo_score_raw.c',
    'src/aymo_score_ref.c',
    'src/aymo_score_vgm.c',
    'src/aymo_tda8425.c',
    'src/aymo_tda8425_common.c',
    'src/aymo_tda8425_dummy.c',
    'src/aymo_tda8425_none.c',
    'src/aymo_wave.c',
    'src/aymo_ym7128.c',
    'src/aymo_ym7128_common.c',
    'src/aymo_ym7128_dummy.c',
    'src/aymo_ym7128_none.c',
    'src/aymo_ymf262.c',
    'src/aymo_ymf262_common.c',
    'src/aymo_ymf262_dummy.c',
    'src/aymo_ymf262_none.c',
  ),

  'AYMO_SOURCES_X86': files(
    'src/aymo_cpu_x86.c',
  ),

  'AYMO_SOURCES_X86_SSE41': files(
    'src/aymo_convert_x86_sse41.c',
    'src/aymo_tda8425_x86_sse41.c',
    'src/aymo_ym7128_x86_sse41.c',
    'src/aymo_ymf262_x86_sse41.c',
  ),

  'AYMO_SOURCES_X86_AVX': files(
    'src/aymo_ym7128_x86_avx.c',
    'src/aymo_ymf262_x86_avx.c',
  ),

  'AYMO_SOURCES_X86_AVX2': files(
    'src/aymo_convert_x86_avx2.c',
    'src/aymo_tda8425_x86_avx2.c',
    'src/aymo_ymf262_x86_avx2.c',
  ),

  'AYMO_SOURCES_ARM': files(
    'src/aymo_cpu_arm.c',
  ),

  'AYMO_SOURCES_ARM_NEON': files(
    'src/aymo_convert_arm_neon.c',
    'src/aymo_tda8425_arm_neon.c',
    'src/aymo_ym7128_arm_neon.c',
    'src/aymo_ymf262_arm_neon.c',
  ),

  'AYMO_SOURCES_LIBC': files(
    'src/aymo_file.c',
  ),

  'AYMO_SOURCES_AYMO': files(
    'src/aymo_empty.c',
  ),
}

# =====================================================================

libm = cc.find_library('m', required: false)

aymo_c_args = []

# Assembly code listings
if cc.get_argument_syntax() == 'msvc'
  if not opt_asm.disabled()
    aymo_c_args += ['/FAcs']
  endif
else
  if opt_asm.enabled()
    aymo_c_args += ['-S', '-fverbose-asm', '-masm=intel', '-Wa,-adhln']
  endif
endif

subdir('contrib')

aymo_sources = sources['AYMO_SOURCES']
aymo_x86_sse41_sources = sources['AYMO_SOURCES_X86_SSE41']
aymo_x86_avx_sources = sources['AYMO_SOURCES_X86_AVX']
aymo_x86_avx2_sources = sources['AYMO_SOURCES_X86_AVX2']
aymo_arm_neon_sources = sources['AYMO_SOURCES_ARM_NEON']

aymo_static_libs = []

foreach intr_name : ['x86_sse41', 'x86_avx', 'x86_avx2', 'arm_neon']
  have_intr = get_variable('aymo_have_@0@'.format(intr_name))
  if have_intr
    intr_sources = get_variable('aymo_@0@_sources'.format(intr_name))
    intr_args = get_variable('aymo_@0@_args'.format(intr_name), [])

    aymo_static_libs += static_library(
      'aymo-static_@0@'.format(intr_name),
      intr_sources,
      c_args: aymo_c_args + intr_args,
      include_directories: [aymo_includes, aymo_contrib_includes],
      link_with: aymo_contrib_lib,
      install: false,
    )
  endif
endforeach


if host_cpu_family in ['x86', 'x86_64']
  aymo_sources += sources['AYMO_SOURCES_X86']
endif
if host_cpu_family in ['arm', 'aarch64']
  aymo_sources += sources['AYMO_SOURCES_ARM']
endif


if host_system in ['windows', 'cygwin']
  aymo_sources += 'src/aymo_sys_windows.c'
elif host_system in ['linux']
  aymo_sources += 'src/aymo_sys_linux.c'
endif


aymo_static_lib = static_library(
  'aymo-static',
  aymo_sources,
  c_args: aymo_c_args,
  include_directories: aymo_includes,
  link_with: aymo_static_libs,
  dependencies: aymo_contrib_dep,
  install: false,
)

aymo_static_dep = declare_dependency(
  include_directories: aymo_includes,
  link_with: aymo_static_lib,
  dependencies: aymo_contrib_dep,
)


aymo_target_lib = library(
  'aymo',
  sources['AYMO_SOURCES_AYMO'],
  version: libversion,
  # darwin_versions: macosversion,  # TODO:
  link_with: aymo_static_lib,
  install: true,
)

aymo_target_dep = declare_dependency(
  include_directories: aymo_includes,
  link_with: aymo_target_lib,
)


aymo_libc_lib = static_library(
  'aymo-libc',
  sources['AYMO_SOURCES_LIBC'],
  c_args: aymo_c_args,
  include_directories: aymo_includes,
  install: false,
)

aymo_libc_dep = declare_dependency(
  include_directories: aymo_includes,
  link_with: aymo_libc_lib,
)

# =====================================================================

# pkg-config files (not using pkg module so we can use the existing .pc.in file)
pkgconf = configuration_data()

pkgconf.set('prefix', join_paths(get_option('prefix')))
pkgconf.set('exec_prefix', '${prefix}')
pkgconf.set('libdir', '${prefix}/@0@'.format(get_option('libdir')))
pkgconf.set('includedir', '${prefix}/@0@'.format(get_option('includedir')))
pkgconf.set('VERSION', aymo_version)
pkgconf.set('URL', aymo_url)

pkg_install_dir = '@0@/pkgconfig'.format(get_option('libdir'))

configure_file(
  input: 'aymo.pc.in',
  output: 'aymo.pc',
  configuration: pkgconf,
  install_dir: pkg_install_dir
)

# =====================================================================

configure_file(
  output: 'aymo_config.h',
  configuration: aymo_conf,
#  macro_name: 'INCLUDE_AYMO_CONFIG_H',
)

subdir('include')

subdir('apps')

if not opt_tests.disabled()
  subdir('tests')
endif

if not opt_benchmarks.disabled()
  subdir('benchmarks')
endif

# =====================================================================

# TODO: Doxygen
#doxygen = find_program('doxygen', required: get_option('docs'))
#if doxygen.found()
#  subdir('doc')
#endif

# =====================================================================

summary(
  {
    'Run-time CPU detection': rtcd_support_names,
    'Generate Assembly Files': opt_asm.enabled(),
  },
  section: 'Compilation',
  bool_yn: true,
  list_sep: ', ',
)

summary(
  {
#    'API documentation': doxygen.found(),  # TODO: Doxygen
    'Apps': not opt_apps.disabled(),
    'Tests': not opt_tests.disabled(),
  },
  section: 'Components',
  bool_yn: true,
  list_sep: ', ',
)
