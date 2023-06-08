dubu_opengl_app = static_library('dubu-opengl-app',
  [
    'src/dubu_opengl_app/AppBase.cpp'
  ],
  cpp_pch: 'pch/pch.h',
  dependencies: [glad_dep, imgui_dep, dubu_log_dep, dubu_window_dep])

dubu_opengl_app_dep = declare_dependency(
  link_with: dubu_opengl_app,
  dependencies: [glad_dep, imgui_dep, dubu_log_dep, dubu_window_dep],
  include_directories: include_directories('./src')
)