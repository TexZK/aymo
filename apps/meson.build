apps_includes = include_directories(
  '.',
)

apps_sources = files(
)

if not opt_apps.disabled()
  app_names = [
    'aymo_ymf262_play',
  ]

  foreach app_name : app_names
    executable(
      app_name,
      apps_sources + files('@0@.c'.format(app_name)),
      include_directories: [apps_includes, aymo_includes],
      link_with: [aymo_static_lib, aymo_libc_lib],
      install: false,
    )
  endforeach
endif
