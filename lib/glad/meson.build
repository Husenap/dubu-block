glad = static_library('glad',
  [
    'src/gl.c'
  ],
  include_directories: include_directories('./include'))

glad_dep = declare_dependency(
  link_with: glad,
  include_directories: include_directories('./include')
)