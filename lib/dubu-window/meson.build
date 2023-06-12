dubu_window = static_library('dubu-window',
  [
    'src/dubu_window/window/GLFWWindow.cpp',
    'src/dubu_window/window/IWindow.cpp',
  ],
  cpp_args: '-DGLFW_INCLUDE_NONE=1',
  dependencies: [glfw_dep, dubu_event_dep],
  cpp_pch: 'pch/pch.h')

dubu_window_dep = declare_dependency(
  compile_args: '-DGLFW_INCLUDE_NONE=1',
  link_with: dubu_window,
  dependencies: [glfw_dep, dubu_event_dep],
  include_directories: include_directories('./src')
)