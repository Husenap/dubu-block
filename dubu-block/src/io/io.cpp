#include "io.hpp"

#include <cstdio>

#include <dubu_log/dubu_log.h>

namespace dubu::block {

std::vector<unsigned char> read_file(std::string_view filepath) {
  FILE* f;

  if (fopen_s(&f, filepath.data(), "rb") != 0) {
    DUBU_LOG_FATAL("Failed to open file: {}", filepath);
  }

  fseek(f, 0, SEEK_END);
  auto size = ftell(f);

  std::vector<unsigned char> content;
  content.resize(size);

  rewind(f);
  fread(content.data(), 1, size, f);

  fclose(f);

  return content;
}

}  // namespace dubu::block