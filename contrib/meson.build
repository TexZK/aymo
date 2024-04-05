#add_project_arguments('-DTDA8425_FLOAT=float')

aymo_contrib_includes = include_directories(
  'Nuked-OPL3',
  'TDA8425_emu/src',
  'YM7128B_emu/src',
)

aymo_contrib_sources = files(
  'Nuked-OPL3/opl3.c',
  'TDA8425_emu/src/TDA8425_emu.c',
  'YM7128B_emu/src/YM7128B_emu.c',
)

aymo_contrib_lib = static_library(
  'aymo-contrib',
  aymo_contrib_sources,
  c_args: aymo_c_args,
  include_directories: aymo_contrib_includes,
  dependencies: libm,
  install: false,
)

aymo_contrib_dep = declare_dependency(
  include_directories: aymo_contrib_includes,
  link_whole: aymo_contrib_lib,
)
