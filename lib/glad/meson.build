glad = static_library('glad',
  [
    'src/glad.c'
  ],
  include_directories: include_directories('./include'))

glad_dep = declare_dependency(
  link_with: glad,
  include_directories: include_directories('./include')
)