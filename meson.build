project('dubu-block', 'c', 'cpp',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'cpp_std=c++20',
    'default_library=static',
    'buildtype=debugoptimized'
  ],
)

glad_dep = subproject('glad').get_variable('glad_dep')
stb_dep = subproject('stb').get_variable('stb_dep')

glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')
imgui_dep = dependency('imgui_docking', default_options:['opengl=true', 'glfw=true'])

subdir('lib')
subdir('dubu-block')