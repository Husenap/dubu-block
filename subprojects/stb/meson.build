project('stb', 'c', 'cpp',
  version : '0.1',
  default_options : 'warning_level=0'
)

stb = static_library('stb', 'src/stb/stb.cpp',
  include_directories: include_directories('./src'))

stb_dep = declare_dependency(
  link_with: stb,
  include_directories: include_directories('./src')
)