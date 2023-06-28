project('dubu-block', 'c', 'cpp',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'default_library=static',
    'cpp_std=c++latest',
    'buildtype=debugoptimized'
  ],
)

if host_machine.system() == 'darwin'
  add_global_arguments('-std=c++20', language : 'cpp')
endif

glad_dep = subproject('glad').get_variable('glad_dep')
stb_dep = subproject('stb').get_variable('stb_dep')

glfw_dep = dependency('glfw3')
glm_dep = dependency('glm')
imgui_dep = dependency('imgui_docking', default_options:['opengl=enabled', 'glfw=enabled'])
fast_noise_lite_dep = dependency('fast_noise_lite')

subdir('lib')
subdir('dubu-block')