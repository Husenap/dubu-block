dubu_block = executable('dubu-block', 
  [
    'src/io/io.cpp',
    'src/main.cpp'
  ],
  cpp_pch: 'pch/pch.h',
  dependencies: [dubu_opengl_app_dep, glm_dep, stb_dep])

install_symlink(
  'assets',
  install_dir: meson.global_build_root(),
  pointing_to: '@0@/assets'.format(meson.current_source_dir())
)