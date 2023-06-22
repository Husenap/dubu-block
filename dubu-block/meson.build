dubu_block = executable('dubu-block', 
  [
    'src/game/chunk_manager.cpp',
    'src/game/chunk.cpp',
    'src/imgui/imgui_curve.cpp',
    'src/io/io.cpp',
    'src/main.cpp'
  ],
  include_directories: include_directories('./src'),
  cpp_pch: 'pch/pch.h',
  dependencies: [dubu_opengl_app_dep, dubu_rect_pack_dep, glm_dep, stb_dep, fast_noise_lite_dep])

install_symlink(
  'assets',
  install_dir: meson.global_build_root(),
  pointing_to: '@0@/assets'.format(meson.current_source_dir())
)