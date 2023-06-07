dubu_window = static_library('dubu-window',
  [
    'src/dubu_window/window/GLFWWindow.cpp',
    'src/dubu_window/window/IWindow.cpp',
  ],
  dependencies: [glfw_dep, dubu_event_dep])

dubu_window_dep = declare_dependency(
  link_with: dubu_window,
  dependencies: [glfw_dep, dubu_event_dep],
  include_directories: include_directories('./src')
)