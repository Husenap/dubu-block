#include "io.hpp"

#include <cstdio>

namespace dubu::block {

std::string read_file(std::string_view filepath) {
  FILE* f;
  fopen_s(&f, filepath.data(), "rb");

  fseek(f, 0, SEEK_END);
  auto size = ftell(f);

  std::string content;
  content.resize(size);

  rewind(f);
  fread(content.data(), 1, size, f);

  fclose(f);

  return content;
}

}  // namespace dubu::block