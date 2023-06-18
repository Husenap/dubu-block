project('dubu-block', 'c', 'cpp',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'cpp_std=c++20',
    'default_library=static'
  ],
)

glfw_dep = declare_dependency(
  dependencies : [dependency('glfw3')],
  compile_args: '-DGLFW_INCLUDE_NONE=1'
)
glm_dep = dependency('glm')
imgui_dep = dependency('imgui')

subdir('lib')
subdir('dubu-block')