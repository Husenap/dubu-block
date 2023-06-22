#include "io.hpp"

#include <fstream>
#include <iterator>
#include <vector>

#include <dubu_log/dubu_log.h>

namespace dubu::block {

std::vector<unsigned char> ReadFile(std::string_view filepath) {
  std::ifstream file(filepath.data(), std::ios_base::binary);

  std::vector<unsigned char> content((std::istreambuf_iterator<char>(file)),
                                     (std::istreambuf_iterator<char>()));

  return content;
}

}  // namespace dubu::block