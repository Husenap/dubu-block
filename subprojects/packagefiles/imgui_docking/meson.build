project(
  'imgui_docking',
  'cpp',
  license: 'MIT',
  version: 'docking',
)

if host_machine.system() == 'darwin'
  add_languages('objcpp')
endif

include_dirs = include_directories('.', 'backends')
sources = files(
  'misc/cpp/imgui_stdlib.cpp',
  'imgui.cpp',
  'imgui_demo.cpp',
  'imgui_draw.cpp',
  'imgui_tables.cpp',
  'imgui_widgets.cpp',
)

cpp = meson.get_compiler('cpp')
dependencies = []

# renderer backends
dependencies += cpp.find_library('d3dcompiler', required: host_machine.system() == 'windows')

libgl_dep = dependency('gl', required: get_option('opengl'))
if libgl_dep.found()
  sources += 'backends/imgui_impl_opengl3.cpp'
  dependencies += libgl_dep
  dependencies += cpp.find_library('dl', required: false)
endif

# platform backends
glfw_dep = dependency('glfw3', required: get_option('glfw'))
if glfw_dep.found()
  sources += 'backends/imgui_impl_glfw.cpp'
  dependencies += glfw_dep
endif


api = '-DIMGUI_API=__attribute__((visibility("default")))'
if host_machine.system() == 'windows'
  api = '-DIMGUI_API=@0@'.format(get_option('default_library') != 'static' ? '__declspec(dllexport)' : '')
endif

imgui = library(
  'imgui',
  sources,
  cpp_args: [api, '-DGLFW_INCLUDE_NONE=1'],
  gnu_symbol_visibility: 'hidden',
  dependencies: dependencies,
  include_directories: include_dirs,
)

if host_machine.system() == 'windows'
  api = '-DIMGUI_API=@0@'.format(get_option('default_library') != 'static' ? '__declspec(dllimport)' : '')
endif

imgui_docking_dep = declare_dependency(
  compile_args: api,
  include_directories: include_dirs,
  link_with: imgui,
)
