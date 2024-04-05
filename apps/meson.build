apps_includes = include_directories(
  '.',
)

apps_sources = files(
)

if not opt_apps.disabled()
  app_name = 'aymo_tda8425_process'
  aymo_tda8425_process_exe = executable(
    app_name,
    apps_sources + files('@0@.c'.format(app_name)),
    c_args: aymo_c_args,
    include_directories: [apps_includes, aymo_includes],
    link_with: [aymo_static_lib, aymo_libc_lib],
    install: false,
  )

  app_name = 'aymo_ym7128_process'
  aymo_ym7128_process_exe = executable(
    app_name,
    apps_sources + files('@0@.c'.format(app_name)),
    c_args: aymo_c_args,
    include_directories: [apps_includes, aymo_includes],
    link_with: [aymo_static_lib, aymo_libc_lib],
    install: false,
  )

  app_name = 'aymo_ymf262_play'
  aymo_ymf262_play_exe = executable(
    app_name,
    apps_sources + files('@0@.c'.format(app_name)),
    c_args: aymo_c_args,
    include_directories: [apps_includes, aymo_includes],
    link_with: [aymo_static_lib, aymo_libc_lib],
    install: false,
  )
endif
