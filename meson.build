# SPDX-License-Identifer: MIT

project('sjp', ['c'],
    meson_version: '>=0.60.0',
    version: files('version.txt'),
)

sjp_includes = include_directories('.')
sjp_sources = []

sjp_sources += files(
    'sjp.c',
)

libsjp = static_library('sjp',
    sjp_sources,
    include_directories: sjp_includes
)

sjp_dep = declare_dependency(
    link_with: libsjp,
    include_directories: sjp_includes
)

meson.override_dependency('sjp', sjp_dep)

subdir('tests')
