dubu_rect_pack = static_library('dubu-rect-pack',
  [
    'src/dubu_rect_pack/packer/Packer.cpp',
    'src/dubu_rect_pack/packer/Space.cpp'
  ],
  include_directories: include_directories('./src'),
  cpp_pch: 'pch/pch.h')

dubu_rect_pack_dep = declare_dependency(
  link_with: dubu_rect_pack,
  include_directories: include_directories('./src')
)