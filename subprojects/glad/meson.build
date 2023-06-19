project('glad', 'c', 'cpp',
  version : '0.1',
  default_options : 'warning_level=0'
)

glad = static_library('glad', 'src/glad.c',
  include_directories: include_directories('./include')
)

glad_dep = declare_dependency(
  link_with: glad,
  include_directories: include_directories('./include')
)